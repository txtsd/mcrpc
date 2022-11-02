#include "mcrpc.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>

#include <filesystem>

#include "JlCompress.h"

mcrpc::mcrpc(QObject *parent)
    : QObject{parent}, qout(stdout), qin(stdin)
{
    // Set .minecraft directory
    dirDotMC = dirHome;
    // Consider using #if defined(Q_OS_LINUX)
    if (platform == "linux")
    {
        if (!dirDotMC.cd(".minecraft"))
        {
            qFatal(".minecraft does not exist!");
        }
    }
    else if (platform == "darwin")
    {
        if (!dirDotMC.cd("Library/Application Support/minecraft"))
        {
            qFatal("~/Library/Application Support/minecraft does not exist!");
        }
    }
    else if (platform == "winnt")
    {
        if (!dirDotMC.cd("AppData/.minecraft"))
        {
            qFatal(".minecraft does not exist!");
        }
    }

    // TODO(txtsd): Use mkdir instead of mkpath
    // Set cache directory
    QDir dirCache(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!dirCache.exists())
    {
        dirCache.mkpath(dirCache.absolutePath());
    }

    // Set jars directory
    if (!dirCache.exists("jars"))
    {
        dirCache.mkpath(dirCache.absolutePath() + "/jars");
    }
    dirJars = dirCache;
    dirJars.cd("jars");

    // Set resource packs directory
    if (!dirDotMC.exists("resourcepacks"))
    {
        dirDotMC.mkpath(dirDotMC.absolutePath() + "/resourcepacks");
    }
    dirRP = dirDotMC;
    dirRP.cd("resourcepacks");
}

void mcrpc::run()
{
    while(true)
    {
        qout << "\n" << "[1] Compare a resource pack against a minecraft version" << "\n";
        qout << "[2] Compare a minecraft version against another minecraft version" << "\n";
        qout << "Choice: ";
        qout.flush();

        QString choiceLogic = qin.readLine();

        if (choiceLogic == '1')
        {
            rp = chooseRP();
            mcv1 = chooseMCV();
            compare(rp, mcv1, mcv2);
            break;
        }
        else if (choiceLogic == '2')
        {
            mcv1 = chooseMCV();
            mcv2 = chooseMCV();
            compare(rp, mcv1, mcv2);
            break;
        }
        else
        {
            qout << "\n" << "Invalid choice!" << "\n";
            qout.flush();
        }
    }

    // Quit after everything is done
    // QCoreApplication::quit();
}

QFileInfo mcrpc::chooseRP()
{
    QFileInfoList listRP = dirRP.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    QFileInfoList listRPChecked;

    for(const QFileInfo &item : listRP)
    {
        if (item.suffix() == "zip")
        {
            QStringList archiveContents = JlCompress::getFileList(item.absoluteFilePath());
            if (archiveContents.contains("pack.mcmeta"))
            {
                listRPChecked.append(item);
            }
        }
        else if (QFileInfo::exists(item.absoluteFilePath() + "/pack.mcmeta"))
        {
            listRPChecked.append(item);
        }
    }

    if (listRPChecked.isEmpty())
    {
        qout << "\n" << "No valid resource packs. Install one and try again.";
        qout.flush();
        QCoreApplication::quit();
    }

    while(true)
    {
        qout << "\n" << "Select the Resource Pack you want to compare:";
        qout.flush();

        int i = 1;
        for(const QFileInfo &item : listRPChecked)
        {
            qout << "\n " << "[" << i << "] " << item.fileName();
            qout.flush();
            i++;
        }
        qout << "\n" << "Resource Pack choice: ";
        qout.flush();

        QString choiceRPNum = qin.readLine();

        i = 1;
        for(const QFileInfo &item : listRPChecked)
        {
            if (QString::number(i) == choiceRPNum)
            {
                return item;
            }
            i++;
        }
        qout << "\n" << "Invalid choice!" << "\n";
    }

}

QJsonObject mcrpc::chooseMCV()
{
    QEventLoop loop;
    QObject::connect(&qnam, &QNetworkAccessManager::finished,
        &loop, &QEventLoop::quit);
    QNetworkReply *reply = qnam.get(QNetworkRequest(linkVM));
    loop.exec();

    QJsonObject jsonObject;

    QUrl url = reply->url();
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response(reply->readAll());
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        jsonObject = jsonResponse.object();

    }
    reply->deleteLater();

    QString choiceStable;
    qout << "\n";
    qout.flush();
    while(true)
    {
        qout << "See non-stable versions? (y/N)" << "\n";
        qout.flush();
        choiceStable = qin.readLine();
        if (choiceStable == "y" || choiceStable == "Y" || choiceStable == "n" || choiceStable == "N")
        {

            break;
        }
    }

    qout << "\n" << "Select the Minecraft Version you want to compare against:" << "\n";
    qout.flush();

    QString choiceMCVNum;
    int counter = 0;
    int counterActual = 1;
    int listSize = 25;
    bool skipDisplay = false;
    while(true)
    {
        bool ok = false;
        for(QJsonValue entry : jsonObject["versions"].toArray())
        {
            counter++;

            if (counter >= jsonObject["versions"].toArray().size())
            {
                skipDisplay = true;
            }

            if (choiceStable == "n" || choiceStable == "N")
            {
                if (entry.toObject()["type"].toString() != "release")
                {
                    continue;
                }
            }

            if (!skipDisplay)
            {
                qout << "  [" << counterActual << "] " << entry.toObject()["id"].toString() << "\n";
                qout.flush();
            }

            if (counterActual % listSize == 0)
            {
                qout << "(Hit Enter to show more versions) Minecraft Version choice: ";
                qout.flush();

                choiceMCVNum = qin.readLine();
                // bool ok;
                choiceMCVNum.toInt(&ok);
                if (ok)
                {
                    break;
                }
            }

            counterActual++;
        }
        if (ok)
        {
            break;
        }
    }

    QJsonObject choiceMCV;
    counterActual = 1;
    for(QJsonValue entry : jsonObject["versions"].toArray())
    {
        if (choiceStable == "n" || choiceStable == "N")
        {
            if (entry.toObject()["type"].toString() != "release")
            {
                continue;
            }
        }

        if (QString::number(counterActual) == choiceMCVNum)
        {
            choiceMCV = entry.toObject();
        }

        counterActual++;
    }

    return choiceMCV;

}

void mcrpc::compare(const QFileInfo &rp, const QJsonObject &mcv1, const QJsonObject &mcv2)
{
    if (rp.exists() && !mcv1.isEmpty() && mcv2.isEmpty())
    {
        getClient(mcv1);
        // setupCompareFolders(true);
        QStringList rpList;
        if (rp.isFile())
        {
            rpList = JlCompress::getFileList(rp.absoluteFilePath());
        }
        else
        {
            QDir selectRPDir = rp.dir();
            selectRPDir.cd(rp.fileName());

            // this is not recursive
            // rpList = selectRPDir.entryList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

            // actually get all files
            for (std::filesystem::recursive_directory_iterator i(selectRPDir.path().toStdString()), end; i != end; ++i)
            {
                QString entry = QString::fromStdString(i->path().string());
                entry.replace(selectRPDir.path() + QDir::separator(), "");
                rpList.append(entry);
            }
        }
        // qDebug() << rpList << "\n" << rpList.size();
        qDebug() << rpList.size();
        QString clientPathStr = dirJars.absoluteFilePath(mcv1["id"].toString() + ".jar");
        QStringList mcv1List = JlCompress::getFileList(clientPathStr);
        QStringList mcv1ListFiltered;
        for (const QString &item : mcv1List)
        {
            if (item.indexOf("assets") == 0)
            {
                mcv1ListFiltered.append(item);
            }
        }
        qDebug() << mcv1ListFiltered.size();
        // Now compare lists and list missing files
        QStringList results;
        for (const QString &item : mcv1ListFiltered)
        {
            if (!rpList.contains(item, Qt::CaseInsensitive))
            {
                results.append(item);
            }
        }
        // qDebug() << "\n" << "Missing Files: " << "\n" << results << "\n" << results.size();
        qDebug() << results.size();
    }
    else if (!rp.exists() && !mcv1.isEmpty() && !mcv2.isEmpty())
    {
        getClient(mcv1);
        getClient(mcv2);
        // TODO(txtsd): Add logic to always compare older against newer
        // setupCompareFolders();
        QString clientPathStr1 = dirJars.absoluteFilePath(mcv1["id"].toString() + ".jar");
        QStringList mcv1List = JlCompress::getFileList(clientPathStr1);
        QStringList mcv1ListFiltered;
        for (const QString &item : mcv1List)
        {
            if (item.indexOf("assets") == 0)
            {
                mcv1ListFiltered.append(item);
            }
        }
        qDebug() << mcv1ListFiltered.size();
        QString clientPathStr2 = dirJars.absoluteFilePath(mcv2["id"].toString() + ".jar");
        QStringList mcv2List = JlCompress::getFileList(clientPathStr2);
        QStringList mcv2ListFiltered;
        for (const QString &item : mcv2List)
        {
            if (item.indexOf("assets") == 0)
            {
                mcv2ListFiltered.append(item);
            }
        }
        qDebug() << mcv2ListFiltered.size();
        QStringList results;
        for (const QString &item : mcv2ListFiltered)
        {
            if (!mcv1ListFiltered.contains(item, Qt::CaseInsensitive))
            {
                results.append(item);
            }
        }
        // qDebug() << "\n" << "Missing Files: " << "\n" << results << "\n" << results.size();
        qDebug() << results.size();
    }
}

void mcrpc::getClient(const QJsonObject &mcv)
{
    QEventLoop loop;
    QObject::connect(&qnam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    QNetworkReply *reply = qnam.get(QNetworkRequest(mcv["url"].toString()));
    loop.exec();

    QJsonObject jsonObject;

    QUrl url = reply->url();
    qDebug() << "url" << url;
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response(reply->readAll());
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        jsonObject = jsonResponse.object();
    }
    else
    {
        qCritical() << "1 Failed to download" << mcv["url"].toString();
    }
    reply->deleteLater();

    // QJsonObject _temp1 = jsonObject["downloads"].toObject();
    // qDebug() << "_temp1" << _temp1;

    QString clientLink = jsonObject["downloads"].toObject()["client"].toObject()["url"].toString();
    QString clientSha = jsonObject["downloads"].toObject()["client"].toObject()["sha1"].toString();

    // Check if these turn up empty
    qDebug() << "clientLink" << clientLink;
    qDebug() << "clientSha" << clientSha;

    QString clientPathStr = dirJars.absoluteFilePath(mcv["id"].toString() + ".jar");
    QFile clientPath(clientPathStr);
    // qDebug() << clientPath;

    QCryptographicHash cryptoHash(QCryptographicHash::Sha1);
    QByteArray hashByteArray = "";
    clientPath.open(QIODevice::ReadOnly);

    if (clientPath.exists())
    {
        qDebug() << "Client" << mcv["id"].toString() << "exists";
        // check and set sha1
        while(!clientPath.atEnd())
        {
            cryptoHash.addData(clientPath.readLine());
        }
        clientPath.close();
        hashByteArray = cryptoHash.result();
    }

    qDebug() << hashByteArray.toHex();

    // if sha1 does not match
    // download client
    if (QString(hashByteArray.toHex()) != clientSha)
    {
        qDebug() << "Downloading" << mcv["id"].toString();
        QEventLoop loop2;
        QNetworkReply *reply2 = qnam.get(QNetworkRequest(clientLink));
        QObject::connect(&qnam, &QNetworkAccessManager::finished,
            &loop2, &QEventLoop::quit);
        QObject::connect(reply2, &QNetworkReply::readyRead,
            [this, &clientPath, &reply2](){
            clientPath.write(reply2->readAll());
        });
        clientPath.open(QIODevice::WriteOnly | QIODevice::Append);
        clientPath.resize(0);
        loop2.exec();
        clientPath.close();
        reply2->deleteLater();
    }

    // TODO(txtsd): Check checksum after downloading
}
