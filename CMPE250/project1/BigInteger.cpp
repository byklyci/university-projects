/*
Student Name: Baran Kılıç
Student Number: 2014400123
Project Number: 1
Operating System: Xubuntu 14.04
Compile Status: 
Program Status: 
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/

#include "BigInteger.h"
#include <algorithm>
/*
ostream &operator<<(ostream &out, const BigInteger &bigInteger)  {
    string str = "";
    Node *head = bigInteger.num->head;
    while (head) {
        str += to_string(head->data);
        head = head->next;
    }
    reverse(str.begin(), str.end());
    if (str == "")
        str = "0";
    out << str;
    return out;
}*/

BigInteger::BigInteger(int number) {
    num = new LinkedList();
    while(number>0){
        num->pushTail(number%10);
        number = number / 10;
    }
}

BigInteger::BigInteger(const string &bigInteger) {
    num = new LinkedList();
    for(int i=0; i<bigInteger.size() ; i++){
        num->pushHead(bigInteger[i]-'0');
    }
}

BigInteger BigInteger::operator+(const BigInteger &list) {
    BigInteger ret;
    *ret.num = (*(this->num) + *(list.num));
    Node *head = ret.num->head;
    while(head != NULL){
        int digit = head->data;
        if(digit > 9){
            if(head->next == NULL){
                ret.num->pushTail(digit/10);
            }else{
                head->next->data = head->next->data + digit/10;
            }
            head->data = digit%10;
        }
        head = head->next;
    }
    return ret;
}

BigInteger BigInteger::operator*(const BigInteger &list) {
    if((this->num->length==1 && this->num->head->data == 0) || (list.num->length==1 && list.num->head->data == 0)){
        BigInteger zero;
        return zero;
    }// return zero if either one is zero
    BigInteger ret;
    Node *head = list.num->head;
    int zeroToAdd = 0;
    while(head != NULL){
        BigInteger multipliedWithDigit(*this * head->data);
        for(int i=0;i<zeroToAdd;i++){//shift number by adding zero to the head
            multipliedWithDigit.num->pushHead(0);
        }
        ret = ret + multipliedWithDigit;
        head = head->next;
        zeroToAdd++;
    }
    return ret;
}

BigInteger BigInteger::operator*(int i) {
    if(i==0){
        BigInteger zero(0);
        return zero;
    }
    BigInteger ret;
    Node *head = this->num->head;
    while(head != NULL){
        ret.num->pushTail((head->data)*i);
        head = head->next;
    }
    head = ret.num->head;
    while(head != NULL){
        int digit = head->data;
        if(digit > 9){
            if(head->next == NULL){
                ret.num->pushTail(digit/10);
            }else{
                head->next->data = head->next->data + digit/10;
            }
            head->data = digit%10;
        }
        head = head->next;
    }
    return ret;
}

BigInteger::BigInteger(const BigInteger &other) {
    num = new LinkedList();
    Node *temp = other.num->head;
    while (temp != NULL) {
        num->pushTail(temp->data);
        temp = temp->next;
    }
}

BigInteger& BigInteger::operator=(const BigInteger &list) {
    if(&list != this){
        delete this->num;
        num = new LinkedList();
        Node *temp = list.num->head;
        while (temp != NULL) {
            num->pushTail(temp->data);
            temp = temp->next;
        }
    }
    return *this;
}

BigInteger::~BigInteger() {
    delete num;
}