TEMPLATE = subdirs
CONFIG += c++17
CONFIG += ordered \
    qt


SUBDIRS +=  win-tester \
            TcpLogClient \
            TcpLogServer \
            console-emul-device \
            console-tester-device \
            console_tester_loop \
            dump_reader
