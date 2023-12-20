#include "mainwindow.h"
#include "qdatetime.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateDatetime);
    timer->start(1000);
    db = new Database();
    db->initialize_database("parkir.db");

       QList<ParkingInfo> parkingInfoList;

       ParkingInfo info1;
       info1.gateSystem = "";
       info1.parkingType = "";
       info1.memberName = "";
       info1.memberExpired = "";
       info1.unitMember = "";
       info1.entryTime = "";
       info1.exitTime = "";
       info1.session = "";
       info1.discount = "";
       info1.promo = "";
       parkingInfoList.append(info1);
       setListMember(parkingInfoList);

       ui->textPlat->setPlaceholderText("Masukan Nomor Kendaraan");
       setupComboBoxes();
       connect(ui->jenisKendaraan, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   this, &MainWindow::comboBoxKendaraanIndexChanged);

       connect(ui->metodPembayaran, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   this, &MainWindow::comboBoxMetodeIndexChanged);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setListMember(const QList<ParkingInfo> &infoList){
    QStandardItemModel *model = new QStandardItemModel(this);

    for (const ParkingInfo &info : infoList) {
        QString itemText = QString("<b>Sistem Gerbang:</b> %1<br>"
                                    "<b>Jenis Parkir:</b> %2<br>"
                                    "<b>Nama Member:</b> %3<br>"
                                    "<b>Expired Member:</b> %4<br>"
                                    "<b>Unit Member:</b> %5<br>"
                                    "<b>Waktu Masuk:</b> %6<br>"
                                    "<b>Waktu Keluar:</b> %7<br>"
                                    "<b>Sesi:</b> %8<br>"
                                    "<b>Promo:</b> %9"
                                    "<b>Diskon:</b> %10")
                                    .arg(info.gateSystem)
                                    .arg(info.parkingType)
                                    .arg(info.memberName)
                                    .arg(info.memberExpired)
                                    .arg(info.unitMember)
                                    .arg(info.entryTime)
                                    .arg(info.exitTime)
                                    .arg(info.session)
                                    .arg(info.promo)
                                    .arg(info.discount);


            QTextDocument *document = new QTextDocument();
            document->setHtml(itemText);

            QTextOption option;
            option.setAlignment(Qt::AlignJustify);
            document->setDefaultTextOption(option);

            QStandardItem *item = new QStandardItem();
            item->setData(document->toPlainText(), Qt::DisplayRole);
            model->appendRow(item);
        }

        ui->informasi->setModel(model);
        ui->informasi->setWordWrap(true);
}
void MainWindow::on_inButton_clicked()
{
    QList<ParkingInfo> parkingInfoList;
    ParkingInfo info;
    if(ui->textPlat->toPlainText() != "" || ui->textPlat->toPlainText() != nullptr){


    qDebug() << "text : "<< ui->textPlat->toPlainText();
    NomorPlat result = parseNomorPlat(ui->textPlat->toPlainText());

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");

    info.gateSystem = "Gerbang A";
    info.parkingType = "Parkir VIP";
    info.memberName = result.kodeWilayah + result.nomorUrut + result.kodeProvinsi;
    info.memberExpired = "";
    info.unitMember = "123";
    info.entryTime = formattedDateTime;
    info.exitTime = "";
    info.session  ="";
    info.discount = "";
    info.promo = "";
    parkingInfoList.append(info);
    setListMember(parkingInfoList);

    if(result.kodeWilayah != "") {
        db->insertDataToParkirTable(
                info.memberName.toStdString(),
                info.gateSystem.toStdString(),
                info.parkingType.toStdString(),
                info.memberExpired.toStdString(),
                info.memberName.toStdString(),
                info.unitMember.toStdString(),
                info.entryTime.toStdString(),
                info.exitTime.toStdString(),
                info.session.toStdString(),
                info.promo.toStdString(),
                info.discount.toStdString());
    }
    qDebug() << "Kode Wilayah: " << result.kodeWilayah;
    qDebug() << "Nomor Urut: " << result.nomorUrut;
    qDebug() << "Kode Provinsi: " << result.kodeProvinsi;


    qDebug() << "Metode yang dipilih: " << metod.metodPembayaran;
    qDebug() << "Mobil yang dipilih: " << metod.mobilTerpilih;
    }
}

void MainWindow::on_outButton_clicked()
{
    if(ui->textPlat->toPlainText() != "" || ui->textPlat->toPlainText() != nullptr){
        qDebug() << "text : "<< ui->textPlat->toPlainText();
        NomorPlat result = parseNomorPlat(ui->textPlat->toPlainText());
        char* plat_temp = ui->textPlat->toPlainText().toUtf8().data();
        std::string resultString;
        resultString = db->getDataAsJsonString(plat_temp);
//        jParse->parsedataFromDb(resultString.c_str());


        const char* jsonString = "Your JSON String Here";

        auto logCallback = [](const char* fieldName, const char* value) {
                qDebug() << QString("%2: %2").arg(fieldName).arg(value);
            };

        int rest = jParse->parsedataFromDb(jsonString, logCallback);

//        qDebug() << QString::fromStdString(resultString).toUtf8().constData();
    }
}
int Database::execute_sql(sqlite3 *db, const char *sql_command) {
    char *error_message = nullptr;
    int result = sqlite3_exec(db, sql_command, nullptr, nullptr, &error_message);

    if (result != SQLITE_OK) {
        qDebug() << "Error executing SQL command: " << error_message ;
        sqlite3_free(error_message);
    }

    return result;
}

void Database::initialize_database(const char *database_name) {
    int result = sqlite3_open(database_name, &db);

    if (result != SQLITE_OK) {
        qDebug() << "Error opening database: " << sqlite3_errmsg(db);
        return;
    }

    const char *create_table_sql = "CREATE TABLE IF NOT EXISTS transaction_data (id INTEGER PRIMARY KEY"\
                                   ", plat_nomor VARCHAR(64) DEFAULT NULL"\
                                   ", sistem_gerbang VARCHAR(64) DEFAULT NULL"\
                                   ", jenis_parkir VARCHAR(64) DEFAULT NULL"\
                                   ", member_expired VARCHAR(64) DEFAULT NULL"\
                                   ", nama_member VARCHAR(64) DEFAULT NULL"\
                                   ", unit_member VARCHAR(64) DEFAULT NULL"\
                                   ", waktu_masuk VARCHAR(64) DEFAULT NULL"\
                                   ", waktu_keluar VARCHAR(64) DEFAULT NULL"\
                                   ", sesi VARCHAR(64) DEFAULT NULL"\
                                   ", promo VARCHAR(64) DEFAULT NULL"\
                                   ", diskon VARCHAR(64) DEFAULT NULL"\
                                   ", stored DATETIME DEFAULT (datetime('now','localtime')))";
    result = execute_sql(db, create_table_sql);

    if (result != SQLITE_OK) {
        qDebug() << "Error creating table: " << sqlite3_errmsg(db);
        sqlite3_close(db);
        return;
    }


}

int Database::insertDataToParkirTable(
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
    const std::string& promo) {

//    std::string sql = "INSERT INTO transaction_data (plat_nomor, sistem_gerbang, jenis_parkir, member_expired, nama_member, unit_member, waktu_masuk, waktu_keluar, sesi, diskon, promo)"\
//                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"\
//                      "WHERE DATE(waktu_masuk) = DATE('now', 'localtime')"\
//                      "AND NOT EXISTS ( "\
//                      "SELECT 1 FROM transaction_data "\
//                      "WHERE plat_nomor = ? AND waktu_keluar IS NULL"\
//                      ");";
    char sql[1024];
    sprintf(sql, "INSERT INTO transaction_data (plat_nomor, sistem_gerbang, jenis_parkir, "\
                 "member_expired, nama_member, unit_member, waktu_masuk, waktu_keluar, sesi, diskon, promo) "\
                 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "\
                 "",platNomor.c_str());
    sqlite3_stmt* stmt;

    qDebug()<<sql;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        qDebug() << "Error preparing INSERT statement: " << sqlite3_errmsg(db);
        return 1;
    }

    sqlite3_bind_text(stmt, 1, platNomor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, sistemGerbang.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, jenisParkir.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, memberExpired.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, namaMember.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, unitMember.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, waktuMasuk.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, waktuKeluar.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, sesi.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, diskon.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, promo.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        qDebug() << "Error inserting data to transaction_data table: " << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return 1;
    } else {
        qDebug() << "Data inserted successfully.";
        sqlite3_finalize(stmt);
        return 0;
    }
}



std::string Database::getDataAsJsonString(const char* nomorPlat) {
    char selectDataSQL[1024] = {0};
    sprintf(selectDataSQL, "SELECT * FROM transaction_data WHERE plat_nomor = '%s' AND waktu_keluar = '' ORDER BY waktu_masuk DESC LIMIT 1", nomorPlat);
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, selectDataSQL, -1, &stmt, 0);

    qDebug() << selectDataSQL;
    if (rc != SQLITE_OK) {
        qDebug() << "Error preparing SELECT statement: " << sqlite3_errmsg(db);
        return "";  // Gagal
    }

    json_t* jsonRoot = json_array();

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        json_t* jsonObj = json_object();

        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            const char* colName = sqlite3_column_name(stmt, i);
            const char* colValue = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));

            json_object_set_new(jsonObj, colName, json_string(colValue));
        }

        json_array_append_new(jsonRoot, jsonObj);
    }

    sqlite3_finalize(stmt);

    char* jsonString = json_dumps(jsonRoot, JSON_INDENT(2));

    json_decref(jsonRoot);

    if (!jsonString) {
        qDebug() << "Error converting JSON to string.";
        return "";  // Gagal
    }

    std::string result(jsonString);

    free(jsonString);

    return result;  // Sukses
}




void MainWindow::updateDatetime(){
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedDateTime = currentDateTime.toString("dd-MM-yyyy hh:mm:ss");
    ui->dateTime->setText(formattedDateTime);
}

int jsonParse::parsedataFromDb(const char* jsonString){
    rapidjson::Document document;
        document.Parse(jsonString);

        if (document.IsArray() && document.Size() >0 ) {
            for (const auto& entry : document.GetArray()) {
                      if (entry.IsObject()) {
                          const rapidjson::Value& id = entry["id"];
                          const rapidjson::Value& platNomor = entry["plat_nomor"];
                          const rapidjson::Value& sistemGerbang = entry["sistem_gerbang"];
                          const rapidjson::Value& jenis_parkir = entry["jenis_parkir"];
                          const rapidjson::Value& nama_member = entry["nama_member"];
                          const rapidjson::Value& unit_member = entry["unit_member"];
                          const rapidjson::Value& waktu_masuk = entry["waktu_masuk"];
                          const rapidjson::Value& waktu_keluar = entry["waktu_keluar"];
                          const rapidjson::Value& sesi = entry["sesi"];
                          const rapidjson::Value& promo = entry["promo"];
                          const rapidjson::Value& diskon = entry["diskon"];
                          const rapidjson::Value& stored = entry["stored"];
                          qDebug() << "ID: " << id.GetString()  ;
                          qDebug() << "Plat Nomor: " << platNomor.GetString() ;
                          qDebug() << "Sistem Gerbang: " << sistemGerbang.GetString() ;
                          qDebug() << jenis_parkir.GetString();
                          qDebug() << nama_member.GetString();
                          qDebug() << unit_member.GetString();
                          qDebug() << waktu_masuk.GetString();
                          qDebug() << waktu_keluar.GetString();
                          qDebug() << sesi.GetString();
                          qDebug() << promo.GetString();
                          qDebug() << diskon.GetString();
                          qDebug() << stored.GetString();
                      }
                  }
        return 0;
        }
    return 1;
}

void jsonParse::logJsonValue(const rapidjson::Value& value, const char* fieldName, std::function<void(const char*, const char*)> logCallback) {
    logCallback(fieldName, value.GetString());
}

int jsonParse::parsedataFromDb(const char* jsonString, std::function<void(const char*, const char*)> logCallback) {
    rapidjson::Document document;
    document.Parse(jsonString);

    if (document.IsArray() && document.Size() > 0) {
        for (const auto& entry : document.GetArray()) {
            if (entry.IsObject()) {
                logJsonValue(entry["id"], "ID", logCallback);
                logJsonValue(entry["plat_nomor"], "Plat Nomor", logCallback);
                logJsonValue(entry["sistem_gerbang"], "Sistem Gerbang", logCallback);
                logJsonValue(entry["jenis_parkir"], "Jenis Parkir", logCallback);
                logJsonValue(entry["nama_member"], "Nama Member", logCallback);
                logJsonValue(entry["unit_member"], "Unit Member", logCallback);
                logJsonValue(entry["waktu_masuk"], "Waktu Masuk", logCallback);
                logJsonValue(entry["waktu_keluar"], "Waktu Keluar", logCallback);
                logJsonValue(entry["sesi"], "Sesi", logCallback);
                logJsonValue(entry["promo"], "Promo", logCallback);
                logJsonValue(entry["diskon"], "Diskon", logCallback);
                logJsonValue(entry["stored"], "Stored", logCallback);
            }
        }
        return 0;
    }
    return 1;
}


NomorPlat MainWindow::parseNomorPlat(const QString &nomorPlat) {
    NomorPlat hasil;

        if (nomorPlat.at(0).isLetter() && nomorPlat.at(1).isDigit() ) {

            hasil.kodeWilayah = nomorPlat.mid(0, 1);
            hasil.nomorUrut = nomorPlat.mid(1, 4);
            hasil.kodeProvinsi = nomorPlat.mid(5, 2);
        } else if (nomorPlat.at(0).isLetter() && nomorPlat.at(1).isLetter() && nomorPlat.at(2).isDigit() ) {

            hasil.kodeWilayah = nomorPlat.mid(0, 2);
            hasil.nomorUrut = nomorPlat.mid(2, 4);
            hasil.kodeProvinsi = nomorPlat.mid(6, 3);
        }
        else {
            qDebug() << "Format nomor plat tidak sesuai.";
            hasil.kodeWilayah = "";
            hasil.nomorUrut = "";
            hasil.kodeProvinsi = "";
        }



    return hasil;
}


void MainWindow::setupComboBoxes() {
    QStringList kendaraan = {"MOBIL", "MOTOR"};
    QStringList metodePembayaran = {"CASH", "eMoney","QRIS"};

    ui->jenisKendaraan->addItems(kendaraan);
    ui->metodPembayaran->addItems(metodePembayaran);
}

void MainWindow::comboBoxKendaraanIndexChanged(int index) {

    metod.mobilTerpilih = ui->jenisKendaraan->itemText(index);
    qDebug() << "Mobil yang dipilih: " << metod.mobilTerpilih;

}

void MainWindow::comboBoxMetodeIndexChanged(int index) {

    metod.metodPembayaran = ui->metodPembayaran->itemText(index);

    qDebug() << "Metode yang dipilih: " << metod.metodPembayaran;
}
