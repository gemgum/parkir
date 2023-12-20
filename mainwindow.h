#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QLabel>
#include <QVBoxLayout>
#include <QDateTime>
#include <QTimer>
#include <string>
#include <functional>


#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "sqlite3.h"
#include "jansson.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


struct ParkingInfo {
    QString gateSystem;
    QString parkingType;
    QString memberName;
    QString memberExpired;
    QString unitMember;
    QString entryTime;
    QString exitTime;
    QString session;
    QString discount;
    QString promo;
};
struct NomorPlat {
    QString kodeWilayah;
    QString nomorUrut;
    QString kodeProvinsi;
};
struct paymentMotode{
    QString mobilTerpilih = "MOBIL";
    QString metodPembayaran = "CASH";
};

class jsonParse{
public:
    int parsedataFromDb(const char* jsonString);
    int parsedataFromDb(const char* jsonString, std::function<void(const char*, const char*)> logCallback);
private:
    void logJsonValue(const rapidjson::Value& value, const char* fieldName, std::function<void(const char*, const char*)> logCallback);
};


class Database{
public:
    int execute_sql(sqlite3 *db, const char *sql_command);
    void initialize_database(const char *database_name);
    int insertDataToParkirTable(
                                 const std::string& platNomor,
                                 const std::string& sistemGerbang,
                                 const std::string& jenisParkir,
                                 const std::string& memberExpired,
                                 const std::string& namaMember,
                                 const std::string& unitMember,
                                 const std::string& waktuMasuk,
                                 const std::string& waktuKeluar,
                                 const std::string& sesi,
                                 const std::string& diskon,
                                 const std::string& promo);
    std::string getDataAsJsonString(const char* nomorPlat);

private:
    sqlite3 *db = NULL;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void displayInfo(const QMap<QString, QString> &infoMap);
    void setListMember(const QList<ParkingInfo> &infoList);
    NomorPlat parseNomorPlat(const QString &nomorPlat);
private slots:
    void comboBoxKendaraanIndexChanged(int index);
    void comboBoxMetodeIndexChanged(int index);
    void on_inButton_clicked();
    void on_outButton_clicked();

private:
    void updateDatetime();
    void setupComboBoxes();
    QStringListModel *model;
    paymentMotode metod;
    Ui::MainWindow *ui;
    Database *db;
    jsonParse *jParse;
};
#endif // MAINWINDOW_H
