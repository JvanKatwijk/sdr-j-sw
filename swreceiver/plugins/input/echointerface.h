
#include	<QString>
#include	<QThread>
class EchoInterface : public QThread {
Q_OBJECT
public:
    virtual ~EchoInterface() {}
    virtual QString echo(const QString &message) = 0;
};

#define EchoInterface_iid "org.qt-project.Qt.Examples.EchoInterface"

Q_DECLARE_INTERFACE(EchoInterface, EchoInterface_iid)

