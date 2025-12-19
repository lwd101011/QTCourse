#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database=QSqlDatabase::addDatabase("QSQLITE");
    QString aFile="E:/QT/QTCourse/lab3/lab4a.db";
    database.setDatabaseName(aFile);

    if(!database.open()){
        qDebug() <<"failed to open database";
    }
    else
        qDebug() <<"open database is ok";
}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this,database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    patientTabModel->setSort(patientTabModel->fieldIndex("name"),Qt::AscendingOrder);
    if(!(patientTabModel->select()))
        return false;

    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),QModelIndex());
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1,1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = patientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID",QUuid::createUuid().toString(QUuid::WithoutBraces));

    patientTabModel->setRecord(curRecNo,curRec);

    return curIndex.row();
}

bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}



bool IDatabase::deleteCurrentPatient()
{
    // QModelIndex curIndex = thePatientSelection->currentIndex();
    // patientTabModel->removeRow(curIndex.row());
    // patientTabModel->submitAll();
    // patientTabModel->select();
    // 检查是否有选中的行
    if (!thePatientSelection->hasSelection()) {
        return false; // 没有选中行，直接返回
    }

    QModelIndex curIndex = thePatientSelection->currentIndex();
    if (curIndex.isValid()) { // 确保索引有效
        patientTabModel->removeRow(curIndex.row());
        return patientTabModel->submitAll(); // 返回提交结果
    }
    return false;
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    return patientTabModel->revertAll();
}



QString IDatabase::userLogin(QString userName, QString password)
{
    //    return "loginOK";
    QSqlQuery query;
    query.prepare("select username,password from user where username = :USER");
    query.bindValue(":USER",userName);
    query.exec();
    if (query.first() && query.value("username").isValid()){
        QString passwd = query.value("password").toString();
        if(passwd == password)
        {
            qDebug() <<"login ok";
            return "loginOK";
        }
        else
        {
            qDebug() <<"wrong password";
            return "wrongPassword";
        }
    }
    else{
        qDebug()<< "no such user";
        return "wrongUsername";
    }

}

IDatabase::IDatabase(QObject *parent) : QObject(parent)
{
    ininDatabase();
}
