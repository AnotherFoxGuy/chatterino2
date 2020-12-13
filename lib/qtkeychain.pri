freebsd {
    INCLUDEPATH += /usr/local/include
    LIBS += -lqt5keychain
} else {
   include(qtkeychain/qt5keychain.pri)
}
