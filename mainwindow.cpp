#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QRegularExpression>

#define INTERVAL 1500

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    labels = new QLabel*[2];// array for labels namely pairs tried and pairs found, below are names
    QString pair_name = "pairs_val";
    QString found_name = "found_val";
    labels[0] = MainWindow::findChild<QLabel*>(pair_name); // putting labels into array
    labels[1] = MainWindow::findChild<QLabel*>(found_name);
    prevButton = new QPushButton; // holds the first button of a pair
    mButtons = new QPushButton*[ROWS*COLS]; // buttons
    mStrings = new QString[ROWS*COLS]; // letters on buttons
    selected = nullptr; // current button that is pushed
    isSecondItemSelected = false; // will handle things
    pairsTried = 0; pairsFound = 0;

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutOccured()));
    resetGame(); // when timer ends, things happen
}

void MainWindow::buttonPressed() { // the process when a button is pressed
    QPushButton *button = (QPushButton *)sender();// take the button
    QRegularExpression reg("\\d+");
    QRegularExpressionMatch match = reg.match(button->objectName());
    int index = match.captured(0).toInt()-1;

    if(isSecondItemSelected || timer->isActive()) {
        return; // ignores pressing a button after two buttons are selected
    }
    if(index == selectedIndex)
        return; // ignores selecting the same button twice

    // if there is no selected item
    if(selected == nullptr){   // objects numbered 1 to 24
        button->setText(mStrings[index]);
        selectedIndex = index;
        selected = mStrings[index];
        prevButton = button;
        return;
    }

    // if there is a selected item and it matches the one selected earlier
    if(index != selectedIndex && QString::compare(selected, mStrings[index]) == 0) {
        button->setText(mStrings[selectedIndex]);
        foundIndices.insert(index);
        foundIndices.insert(selectedIndex);
        disableButtons(button,prevButton); // buttons can't be pressed anymore
        selected = nullptr;
        prevButton = nullptr;
        selectedIndex = -1;
        isSecondItemSelected = false;
        pairsFound++;
        pairsTried++;
        labels[1]->setText(QString::number(pairsFound));
        labels[0]->setText(QString::number(pairsTried));
        timer->start(INTERVAL/1000);
        return;
    }
    // if there are two selected items but not more
    // this makes sure not more than 2 items gets selected
    if(!isSecondItemSelected && prevButton != nullptr){
        button->setText(mStrings[index]);
        isSecondItemSelected = true;
        pairsTried++;
        labels[0]->setText(QString::number(pairsTried));
        timer->start(INTERVAL/2);// show user the letters for a while to remember and don't take input in the meantime
    }
}
void MainWindow::disableButtons(QPushButton* button1, QPushButton* button2) {
    button1->setStyleSheet("background-color: black");
    button2->setStyleSheet("background-color: black");
    button1->setDisabled(true);
    button2->setDisabled(true);
    return;
}
void MainWindow::timeoutOccured() {
    isSecondItemSelected = false;
    selected = nullptr;
    prevButton = nullptr;
    selectedIndex = -1;
    hideButtonTexts();
    timer->stop();
}

void MainWindow::findButtons() {
    // buttons are active and will call buttonPressed when they are released
    // get a handle for each button
    for(int i = 0; i < ROWS*COLS; i++){
        QString buttonName = "pushButton_" + QString::number(i+1);
        mButtons[i] = MainWindow::findChild<QPushButton*>(buttonName);
        connect(mButtons[i], SIGNAL(released()), this, SLOT(buttonPressed()));
    }
}

void MainWindow::resetGame() {
    // restart game
    findButtons();
    // give buttons values
    setRandomButtonTexts();
    pairsFound = 0;
    pairsTried = 0;
    labels[1]->setText(QString::number(0));
    labels[0]->setText(QString::number(0));
    selected = nullptr;
    selectedIndex = -1;
    isSecondItemSelected = false;
    foundIndices.clear();
    for(int i = 0; i < ROWS*COLS; i++) {
        mButtons[i]->setStyleSheet("background-color: silver");
        mButtons[i]->setVisible(true);
        mButtons[i]->setDisabled(false);
    }
    timer->start(INTERVAL);
}

void MainWindow::setRandomButtonTexts() {
    QSet<char> usedChars;
    QSet<int> usedIndices;
    // reset the buttons with random chars
    for(int i = 0; i < ROWS*COLS*0.5; i++) {
        // get a unique char
        char c = 'a';
        do {
            c = static_cast<char>(QRandomGenerator::global()->bounded('A', 'Z'+1));
        }while(usedChars.contains(c));
        usedChars.insert(c);
        mButtons[i]->setText(QString(c));
        mStrings[i] = QString(c);

        // get a unique pair
        int pairIndex = 0;
        do {
            pairIndex = static_cast<int>(QRandomGenerator::global()->bounded(ROWS*COLS/2, ROWS*COLS));
        }while(usedIndices.contains(pairIndex));
        usedIndices.insert(pairIndex);
        mButtons[pairIndex]->setText(QString(c));        
        mStrings[pairIndex] = QString(c);
    }
}

void MainWindow::hideButtonTexts() {
    assert(mButtons != nullptr);
    for(int i = 0; i < ROWS*COLS; i++) {
        if(!foundIndices.contains(i))
            mButtons[i]->setText("");
    }
}

void MainWindow::revealButtonTexts() {
    assert(mButtons != nullptr);
    for(int i = 0; i < ROWS*COLS; i++) {
        mButtons[i]->setText(mStrings[i]);
    }
}

void MainWindow::on_Reset_released()
{
    resetGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}


