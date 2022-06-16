/********************************************************************************
** Form generated from reading UI file 'edit_match_dlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDIT_MATCH_DLG_H
#define UI_EDIT_MATCH_DLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EditMatchDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *tlPlayer1_1;
    QLabel *tlPlayer2_1;
    QLabel *tlPlayer1_2;
    QLabel *tlPlayer2_2;
    QLabel *tlPlayer1_3;
    QLabel *tlPlayer2_3;
    QHBoxLayout *horizontalLayout_0;
    QSpacerItem *h_spacer_0;
    QLineEdit *lePtsLt;
    QLabel *tlColon;
    QHBoxLayout *horizontalLayout_1;
    QLineEdit *lePtsRt;
    QSpacerItem *h_spacer_1;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *EditMatchDialog)
    {
        if (EditMatchDialog->objectName().isEmpty())
            EditMatchDialog->setObjectName(QString::fromUtf8("EditMatchDialog"));
        EditMatchDialog->resize(475, 166);
        verticalLayout = new QVBoxLayout(EditMatchDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tlPlayer1_1 = new QLabel(EditMatchDialog);
        tlPlayer1_1->setObjectName(QString::fromUtf8("tlPlayer1_1"));
        QFont font;
        font.setPointSize(11);
        tlPlayer1_1->setFont(font);
        tlPlayer1_1->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(tlPlayer1_1, 0, 0, 1, 1);

        tlPlayer2_1 = new QLabel(EditMatchDialog);
        tlPlayer2_1->setObjectName(QString::fromUtf8("tlPlayer2_1"));
        tlPlayer2_1->setFont(font);
        tlPlayer2_1->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(tlPlayer2_1, 0, 2, 1, 1);

        tlPlayer1_2 = new QLabel(EditMatchDialog);
        tlPlayer1_2->setObjectName(QString::fromUtf8("tlPlayer1_2"));
        tlPlayer1_2->setFont(font);
        tlPlayer1_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(tlPlayer1_2, 1, 0, 1, 1);

        tlPlayer2_2 = new QLabel(EditMatchDialog);
        tlPlayer2_2->setObjectName(QString::fromUtf8("tlPlayer2_2"));
        tlPlayer2_2->setFont(font);
        tlPlayer2_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(tlPlayer2_2, 1, 2, 1, 1);

        tlPlayer1_3 = new QLabel(EditMatchDialog);
        tlPlayer1_3->setObjectName(QString::fromUtf8("tlPlayer1_3"));
        tlPlayer1_3->setFont(font);
        tlPlayer1_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(tlPlayer1_3, 2, 0, 1, 1);

        tlPlayer2_3 = new QLabel(EditMatchDialog);
        tlPlayer2_3->setObjectName(QString::fromUtf8("tlPlayer2_3"));
        tlPlayer2_3->setFont(font);
        tlPlayer2_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(tlPlayer2_3, 2, 2, 1, 1);

        horizontalLayout_0 = new QHBoxLayout();
        horizontalLayout_0->setObjectName(QString::fromUtf8("horizontalLayout_0"));
        h_spacer_0 = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_0->addItem(h_spacer_0);

        lePtsLt = new QLineEdit(EditMatchDialog);
        lePtsLt->setObjectName(QString::fromUtf8("lePtsLt"));
        lePtsLt->setFont(font);
        lePtsLt->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_0->addWidget(lePtsLt);


        gridLayout->addLayout(horizontalLayout_0, 3, 0, 1, 1);

        tlColon = new QLabel(EditMatchDialog);
        tlColon->setObjectName(QString::fromUtf8("tlColon"));

        gridLayout->addWidget(tlColon, 3, 1, 1, 1);

        horizontalLayout_1 = new QHBoxLayout();
        horizontalLayout_1->setObjectName(QString::fromUtf8("horizontalLayout_1"));
        lePtsRt = new QLineEdit(EditMatchDialog);
        lePtsRt->setObjectName(QString::fromUtf8("lePtsRt"));
        lePtsRt->setFont(font);

        horizontalLayout_1->addWidget(lePtsRt);

        h_spacer_1 = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_1->addItem(h_spacer_1);


        gridLayout->addLayout(horizontalLayout_1, 3, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 18, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(EditMatchDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(EditMatchDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), EditMatchDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), EditMatchDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(EditMatchDialog);
    } // setupUi

    void retranslateUi(QDialog *EditMatchDialog)
    {
        EditMatchDialog->setWindowTitle(QCoreApplication::translate("EditMatchDialog", "Match", nullptr));
        tlPlayer1_1->setText(QCoreApplication::translate("EditMatchDialog", "Spieler11", nullptr));
        tlPlayer2_1->setText(QCoreApplication::translate("EditMatchDialog", "Spieler21", nullptr));
        tlPlayer1_2->setText(QCoreApplication::translate("EditMatchDialog", "Spieler12", nullptr));
        tlPlayer2_2->setText(QCoreApplication::translate("EditMatchDialog", "Spieler22", nullptr));
        tlPlayer1_3->setText(QCoreApplication::translate("EditMatchDialog", "Spieler13", nullptr));
        tlPlayer2_3->setText(QCoreApplication::translate("EditMatchDialog", "Spieler23", nullptr));
        tlColon->setText(QCoreApplication::translate("EditMatchDialog", ":", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EditMatchDialog: public Ui_EditMatchDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDIT_MATCH_DLG_H
