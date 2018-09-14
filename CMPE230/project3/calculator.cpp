// written by Baran Kılıç, Mehmet Hakan Kurtoğlu

#include <QPushButton>
#include <QButtonGroup>
#include <QLCDNumber>
#include <QVBoxLayout>
#include <QGridLayout>
#include "calculator.h"

Calculator::Calculator(QWidget *parent): QWidget(parent)
{
	buttongroup = new QButtonGroup;
	disp = new QLCDNumber;
	disp->setHexMode();
	disp->setMinimumHeight(36);

	// this will hold first number entered, i.e. left operand 
	sumSoFar = 0;
	// this will hold last number entered, i.e. right operand
	currentNum = 0;
	// when operation is adding
	plusOperator = false;
	// when operation is subtracting
	minusOperator = false;

	button[0] = new QPushButton("+");
	button[1] = new QPushButton("-");
	button[2] = new QPushButton("=");
	button[3] = new QPushButton("Clr");

	// adding number buttons
	QString s;
	for(int i=4 ; i < 14 ; i++) {
		s = QString::number(i-4) ; 
		button[i] = new QPushButton(s);
	}

	// hex digits
	button[14] = new QPushButton("A");
	button[15] = new QPushButton("B");
	button[16] = new QPushButton("C");
	button[17] = new QPushButton("D");
	button[18] = new QPushButton("E");
	button[19] = new QPushButton("F");

	QGridLayout *grid = new QGridLayout;

	for(int i=0 ; i < 20 ; i++) {
		button[i]->setMinimumHeight(30);
		grid->addWidget(button[i],(i/4),(i%4));
		buttongroup->addButton(button[i],i+1) ; 
	}

	connect(buttongroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));

	QVBoxLayout *vertLayout = new QVBoxLayout;
	vertLayout->addWidget(disp);
	vertLayout->addLayout(grid);

	setLayout(vertLayout);
	
	setWindowTitle(tr("Hex Calculator"));
}

void Calculator::buttonClicked(int buttonid) {
	// if first button, it means addition and currentNum is stored in sumSoFar and gets value zero
	if(buttonid==1){ // +
		plusOperator=true;
		minusOperator=false;
		sumSoFar=currentNum;
		currentNum=0;
		disp->display(currentNum);
	// if first button, it means subtracting and currentNum is stored in sumSoFar and gets value zero
	}else if(buttonid==2){ // -
		minusOperator=true;
		plusOperator=false;
		sumSoFar=currentNum;
		currentNum=0;
		disp->display(currentNum);
	// ends the operation and displays result
	}else if(buttonid==3){ // =
		if(plusOperator){
			sumSoFar += currentNum;
		}else if(minusOperator){
			sumSoFar -= currentNum;
		}
		plusOperator = false;
		minusOperator = false;
		disp->display(sumSoFar);
		currentNum = 0;
	// clears all stored values
	}else if(buttonid==4){ // clr
		plusOperator = false;
		minusOperator = false;
		currentNum = 0;
		sumSoFar = 0;
		disp->display(currentNum);
	}else if(buttonid>4 && buttonid <21){ // numbers 0-9 A-F
		int num = buttonid-5;
		currentNum = currentNum*16 + num;
		disp->display(currentNum);
	}
}
