// written by Baran Kılıç, Mehmet Hakan Kurtoğlu
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>

class QButtonGroup;
class QPushButton;
class QLCDNumber;

class Calculator : public QWidget
{
    Q_OBJECT

public:
	Calculator(QWidget *parent = 0);

private slots:
	void buttonClicked(int buttonid);

private:
	int sumSoFar;
	int currentNum;
	bool plusOperator;
	bool minusOperator;

	QButtonGroup *buttongroup; 
	QPushButton *button[20];
	QLCDNumber *disp; 

};

#endif
