#include "twitch-eventsub-ws/session.hpp"

#include "twitch-eventsub-ws/detail/errors.hpp"
#include "twitch-eventsub-ws/listener.hpp"
#include "twitch-eventsub-ws/messages/metadata.hpp"
#include "twitch-eventsub-ws/payloads/channel-ban-v1.hpp"
#include "twitch-eventsub-ws/payloads/session-welcome.hpp"

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/json.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;

namespace chatterino::eventsub::lib {

// Subscription Type + Subscription Version
using EventSubSubscription = std::pair<std::string, std::string>;

using NotificationHandlers = std::unordered_map<
    EventSubSubscription,
    std::function<boost::system::error_code(
        messages::Metadata, boost::json::value, std::unique_ptr<Listener> &)>,
    boost::hash<EventSubSubscription>>;

using MessageHandlers =
    std::unordered_map<std::string, std::function<boost::system::error_code(
                                        messages::Metadata, boost::json::value,
                                        std::unique_ptr<Listener> &,
                                        const NotificationHandlers &)>>;

namespace {

    // Report a failure
    void fail(beast::error_code ec, char const *what)
    {
        std::cerr << what << ": " << ec.message() << " (" << ec.location()
                  << ")\n";
    }

    template <class T>
    boost::system::result<T> parsePayload(const boost::json::value &jv)
    {
        auto result = boost::json::try_value_to<T>(jv);
        if (!result.has_value())
        {
            return result.error();
        }

        return std::move(result.value());
    }

    // Subscription types
    const NotificationHandlers NOTIFICATION_HANDLERS{
        {
            {"channel.ban", "1"},
            [](const auto &metadata, const auto &jv, auto &listener) {
                auto oPayload =
                    parsePayload<payload::channel_ban::v1::Payload>(jv);
                if (!oPayload)
                {
                    return oPayload.error();
                }
                listener->onChannelBan(metadata, *oPayload);
                return boost::system::error_code{};
            },
        },
        {
            {"stream.online", "1"},
            [](const auto &metadata, const auto &jv, auto &listener) {
                auto oPayload =
                    parsePayload<payload::stream_online::v1::Payload>(jv);
                if (!oPayload)
                {
                    return oPayload.error();
                }
                listener->onStreamOnline(metadata, *oPayload);
                return boost::system::error_code{};
            },
        },
        {
            {"stream.offline", "1"},
            [](const auto &metadata, const auto &jv, auto &listener) {
                auto oPayload =
                    parsePayload<payload::stream_offline::v1::Payload>(jv);
                if (!oPayload)
                {
                    return oPayload.error();
                }
                listener->onStreamOffline(metadata, *oPayload);
                return boost::system::error_code{};
            },
        },
        {
            {"channel.chat.notification", "1"},
            [](const auto &metadata, const auto &jv, auto &listener) {
                auto oPayload = parsePayload<
                    payload::channel_chat_notification::v1::Payload>(jv);
                if (!oPayload)
                {
                    return oPayload.error();
                }
                listener->onChannelChatNotification(metadata, *oPayload);
                return boost::system::error_code{};
            },
        },
        {
            {"channel.update", "1"},
            [](const auto &metadata, const auto &jv, auto &listener) {
                auto oPayload =
                    parsePayload<payload::channel_update::v1::Payload>(jv);
                if (!oPayload)
                {
                    return oPayload.error();
                }
                listener->onChannelUpdate(metadata, *oPayload);
                return boost::system::error_code{};
            },
        },
        {
            {"channel.chat.message", "1"},
            [](const auto &metadata, const auto &jv, auto &listener) {
                auto oPayload =
                    parsePayload<payload::channel_chat_message::v1::Payload>(
                        jv);
                if (!oPayload)
                {
                    return oPayload.error();
                }
                listener->onChannelChatMessage(metadata, *oPayload);
                return boost::system::error_code{};
            },
        },
        {
            {"channel.moderate", "2"},
            [](const auto &metadata, const auto &jv, auto &listener) {
                auto oPayload =
                    parsePayload<payload::channel_moderate::v2::Payload>(jv);
                if (!oPayload)
                {
                    return oPayload.error();
                }
                listener->onChannelModerate(metadata, std::move(*oPayload));
                return boost::system::error_code{};
            },
        },
        // Add your new subscription types above this line
    };

    const MessageHandlers MESSAGE_HANDLERS{
        {
            "session_welcome",
            [](const auto &metadata, const auto &jv, auto &listener,
               const auto & /*notificationHandlers*/) {
                auto oPayload =
                    parsePayload<payload::session_welcome::Payload>(jv);
                if (!oPayload)
                {
                    // TODO: error handling
                    return oPayload.error();
                }
                const auto &payload = *oPayload;

                listener->onSessionWelcome(metadata, payload);
                return boost::system::error_code{};
            },
        },
        {
            "session_keepalive",
            [](const auto &metadata, const auto &jv, auto &listener,
               const auto &notificationHandlers) {
                // TODO: should we do something here?
                return boost::system::error_code{};
            },
        },
        {
            "notification",
            [](const auto &metadata, const auto &jv, auto &listener,
               const auto &notificationHandlers) {
                listener->onNotification(metadata, jv);

                if (!metadata.subscriptionType || !metadata.subscriptionVersion)
                {
                    // TODO: error handling
                    return boost::system::error_code{};
                }

                auto it =
                    notificationHandlers.find({*metadata.subscriptionType,
                                               *metadata.subscriptionVersion});
                if (it == notificationHandlers.end())
                {
                    EVENTSUB_BAIL_HERE(error::Kind::NoMessageHandler);
                }

                return it->second(metadata, jv, listener);
            },
        },
    };

}  // namespace

boost::system::error_code handleMessage(std::unique_ptr<Listener> &listener,
                                        const beast::flat_buffer &buffer)
{
    boost::system::error_code parseError;
    auto jv =
        boost::json::parse(beast::buffers_to_string(buffer.data()), parseError);
    if (parseError)
    {
        // TODO: wrap error?
        return parseError;
    }

    const auto *jvObject = jv.if_object();
    if (jvObject == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::ExpectedObject);
    }

    const auto *metadataV = jvObject->if_contains("metadata");
    if (metadataV == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }
    auto metadataResult =
        boost::json::try_value_to<messages::Metadata>(*metadataV);
    if (metadataResult.has_error())
    {
        // TODO: wrap error?
        return metadataResult.error();
    }

    const auto &metadata = metadataResult.value();

    auto handler = MESSAGE_HANDLERS.find(metadata.messageType);

    if (handler == MESSAGE_HANDLERS.end())
    {
        EVENTSUB_BAIL_HERE(error::Kind::NoMessageHandler);
    }

    const auto *payloadV = jvObject->if_contains("payload");
    if (payloadV == nullptr)
    {
        EVENTSUB_BAIL_HERE(error::Kind::FieldMissing);
    }

    return handler->second(metadata, *payloadV, listener,
                           NOTIFICATION_HANDLERS);
}

// Resolver and socket require an io_context
Session::Session(boost::asio::io_context &ioc, boost::asio::ssl::context &ctx,
                 std::unique_ptr<Listener> listener)
    : resolver(boost::asio::make_strand(ioc))
    , ws(boost::asio::make_strand(ioc), ctx)
    , listener(std::move(listener))
{
}

// Start the asynchronous operation
void Session::run(std::string _host, std::string _port, std::string _path,
                  std::string _userAgent)
{
    // Save these for later
    this->host = std::move(_host);
    this->port = std::move(_port);
    this->path = std::move(_path);
    this->userAgent = std::move(_userAgent);

    // Look up the domain name
    this->resolver.async_resolve(
        this->host, this->port,
        beast::bind_front_handler(&Session::onResolve, shared_from_this()));
}

void Session::close()
{
    boost::beast::websocket::close_reason closeReason("Shutting down");

    // TODO: Test this with a misbehaving eventsub server that doesn't respond to our close
    this->ws.async_close(
        closeReason,
        beast::bind_front_handler(&Session::onClose, shared_from_this()));
}

Listener *Session::getListener()
{
    return this->listener.get();
}

void Session::onResolve(beast::error_code ec,
                        boost::asio::ip::tcp::resolver::results_type results)
{
    if (ec)
    {
        return fail(ec, "resolve");
    }

    // Set a timeout on the operation
    beast::get_lowest_layer(this->ws).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(this->ws).async_connect(
        results,
        beast::bind_front_handler(&Session::onConnect, shared_from_this()));
}

void Session::onConnect(
    beast::error_code ec,
    boost::asio::ip::tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
    {
        return fail(ec, "connect");
    }

    // Set a timeout on the operation
    beast::get_lowest_layer(this->ws).expires_after(std::chrono::seconds(30));

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(this->ws.next_layer().native_handle(),
                                  this->host.c_str()))
    {
        ec = beast::error_code(static_cast<int>(::ERR_get_error()),
                               boost::asio::error::get_ssl_category());
        return fail(ec, "connect");
    }

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    host += ':' + std::to_string(ep.port());

    // Perform the SSL handshake
    this->ws.next_layer().async_handshake(
        boost::asio::ssl::stream_base::client,
        beast::bind_front_handler(&Session::onSSLHandshake,
                                  shared_from_this()));
}

void Session::onSSLHandshake(beast::error_code ec)
{
    if (ec)
    {
        return fail(ec, "ssl_handshake");
    }

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(this->ws).expires_never();

    // Set suggested timeout settings for the websocket
    this->ws.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    this->ws.set_option(websocket::stream_base::decorator(
        [userAgent{this->userAgent}](websocket::request_type &req) {
            req.set(http::field::user_agent, userAgent);
        }));

    // Perform the websocket handshake
    this->ws.async_handshake(
        this->host, this->path,
        beast::bind_front_handler(&Session::onHandshake, shared_from_this()));
}

void Session::onHandshake(beast::error_code ec)
{
    if (ec)
    {
        return fail(ec, "handshake");
    }

    this->ws.async_read(buffer, beast::bind_front_handler(&Session::onRead,
                                                          shared_from_this()));
}

void Session::onRead(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        return fail(ec, "read");
    }

    auto messageError = handleMessage(this->listener, this->buffer);
    if (messageError)
    {
        fail(messageError, "handleMessage");
    }

    this->buffer.clear();

    this->ws.async_read(buffer, beast::bind_front_handler(&Session::onRead,
                                                          shared_from_this()));
}

/**
    this->ws_.async_close(
        websocket::close_code::normal,
        beast::bind_front_handler(&Session::onClose, shared_from_this()));
        */
void Session::onClose(beast::error_code ec)
{
    if (ec)
    {
        return fail(ec, "close");
    }

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    std::cout << beast::make_printable(buffer.data()) << std::endl;
}

}  // namespace chatterino::eventsub::lib
