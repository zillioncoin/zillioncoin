#ifndef MININGPAGE_H
#define MININGPAGE_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <memory>

#include "walletmodel.h"

#include <QPropertyAnimation>

namespace Ui {
class MiningPage;
}

class MiningPage : public QWidget
{
    Q_OBJECT

public:
    explicit MiningPage(QWidget *parent = 0);
    ~MiningPage();

    void setModel(WalletModel *model);

    QPushButton* ButtonSPREAD;
    QPushButton* ButtonBLAKE;
    QPushButton* ButtonJH;
    QPushButton* ButtonKECCAK;
    QPushButton* ButtonSHAVITE;
    QPushButton* ButtonECHO;

    QLabel* BGLINE;

    QPropertyAnimation *animation;
    QPropertyAnimation *animation2;
    QPropertyAnimation *animation3;

private:
    Ui::MiningPage *ui;
    WalletModel *model;
    std::auto_ptr<WalletModel::UnlockContext> unlockContext;
    bool hasMiningprivkey;

    void restartMining(bool fGenerate);
    void timerEvent(QTimerEvent *event);
    void updateUI();

private slots:

    void changeNumberOfCores(int i);
    void switchMining();
};

#endif // MININGPAGE_H
