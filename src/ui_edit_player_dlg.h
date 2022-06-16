/********************************************************************************
** Form generated from reading UI file 'edit_player_dlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDIT_PLAYER_DLG_H
#define UI_EDIT_PLAYER_DLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_EditPlayerDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *tlVorname;
    QLineEdit *leVorname;
    QLabel *tlName;
    QLineEdit *leName;
    QLabel *tlAssoc;
    QLineEdit *leAssoc;
    QLabel *tlPoints;
    QLineEdit *lePoints;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *EditPlayerDialog)
    {
        if (EditPlayerDialog->objectName().isEmpty())
            EditPlayerDialog->setObjectName(QString::fromUtf8("EditPlayerDialog"));
        EditPlayerDialog->resize(510, 146);
        gridLayout = new QGridLayout(EditPlayerDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tlVorname = new QLabel(EditPlayerDialog);
        tlVorname->setObjectName(QString::fromUtf8("tlVorname"));
        tlVorname->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(tlVorname, 0, 0, 1, 1);

        leVorname = new QLineEdit(EditPlayerDialog);
        leVorname->setObjectName(QString::fromUtf8("leVorname"));

        gridLayout->addWidget(leVorname, 0, 1, 1, 1);

        tlName = new QLabel(EditPlayerDialog);
        tlName->setObjectName(QString::fromUtf8("tlName"));
        tlName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(tlName, 1, 0, 1, 1);

        leName = new QLineEdit(EditPlayerDialog);
        leName->setObjectName(QString::fromUtf8("leName"));

        gridLayout->addWidget(leName, 1, 1, 1, 1);

        tlAssoc = new QLabel(EditPlayerDialog);
        tlAssoc->setObjectName(QString::fromUtf8("tlAssoc"));
        tlAssoc->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(tlAssoc, 2, 0, 1, 1);

        leAssoc = new QLineEdit(EditPlayerDialog);
        leAssoc->setObjectName(QString::fromUtf8("leAssoc"));

        gridLayout->addWidget(leAssoc, 2, 1, 1, 1);

        tlPoints = new QLabel(EditPlayerDialog);
        tlPoints->setObjectName(QString::fromUtf8("tlPoints"));
        tlPoints->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(tlPoints, 3, 0, 1, 1);

        lePoints = new QLineEdit(EditPlayerDialog);
        lePoints->setObjectName(QString::fromUtf8("lePoints"));
        lePoints->setMaxLength(5);

        gridLayout->addWidget(lePoints, 3, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 18, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 4, 1, 1, 1);

        buttonBox = new QDialogButtonBox(EditPlayerDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 5, 0, 1, 2);


        retranslateUi(EditPlayerDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), EditPlayerDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), EditPlayerDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(EditPlayerDialog);
    } // setupUi

    void retranslateUi(QDialog *EditPlayerDialog)
    {
        EditPlayerDialog->setWindowTitle(QCoreApplication::translate("EditPlayerDialog", "Spieler", nullptr));
        tlVorname->setText(QCoreApplication::translate("EditPlayerDialog", "Vorname", nullptr));
        tlName->setText(QCoreApplication::translate("EditPlayerDialog", "Name", nullptr));
        tlAssoc->setText(QCoreApplication::translate("EditPlayerDialog", "Verein/Team", nullptr));
        tlPoints->setText(QCoreApplication::translate("EditPlayerDialog", "Punktzahl", nullptr));
        lePoints->setInputMask(QString());
    } // retranslateUi

};

namespace Ui {
    class EditPlayerDialog: public Ui_EditPlayerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDIT_PLAYER_DLG_H
