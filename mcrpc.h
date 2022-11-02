#ifndef MCRPC_H
#define MCRPC_H

#include <QDir>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QString>


class mcrpc : public QObject
{
    Q_OBJECT

    const QString platform = QSysInfo::kernelType();
    const QDir dirHome = QDir::home();
    QDir dirDotMC;
    QDir dirCache;
    QDir dirJars;
    QDir dirRP;
    const QString linkVM =
        "https://launchermeta.mojang.com/mc/game/version_manifest_v2.json";
    QTextStream qout;
    QTextStream qin;
    QNetworkAccessManager qnam;
    QFileInfo rp;
    QJsonObject mcv1;
    QJsonObject mcv2;
    QFile clientPath;

    QFileInfo chooseRP();
    QJsonObject chooseMCV();
    void compare(const QFileInfo &rp, const QJsonObject &mcv1, const QJsonObject &mcv2);
    void getClient(const QJsonObject &mcv);

public:
    explicit mcrpc(QObject *parent = nullptr);

signals:

public slots:
    void run();

};

#endif // MCRPC_H
