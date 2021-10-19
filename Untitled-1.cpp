#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QTGui>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class form : public QMainWindow {
Q_OBJECT
public:
// максимальное количество амплитуд
int km = 30;
// период
double period = 0;
// массив значений функции u(t)
vector <double> u;
// массив амплитуд
vector <double> U;
double pi = 3.1415926;
// размеры окна
int w = 800;
int h = 800;
// кнопка "Open"
QPushButton* but;

form(){
    setWindowTitle("Spectr");
    resize(w,h);
    but = new QPushButton(this);
    but->move(30,30);
    but->setText("Open");
    but->adjustSize();
    connect(but, SIGNAL(clicked()),
    this, SLOT(click()));
}
public slots:

// действие щелчок по кнопке

void click(){
    // выбранное имя файла
    QString name =
    QFileDialog::getOpenFileName(this,
    "Open File");
    if (name == "") return;
    // чтение данных
    load(string(name.toLocal8Bit()));
    if (u.size() == 0) return;
    // вычисление спектра
    spectr();
    }

// чтение данных из файла

void load(string name){
    u.resize(0);
    ifstream in(name);
    string c;
// чтение периода
if (getline(in,c)){
    stringstream ss(c);
    ss >> period;
    }
else
return;
// чтение значений периодической функции u(t)
int n = 0;
while (getline(in,c)){
    stringstream ss(c);
    n++;
    u.resize(n);
    ss >> u[n-1];
    }
}

// вычисление спектра

void spectr(){
    U.resize(km+1);
    for (int k = 0; k <= km; k++)
    U[k] = amplitude(k);
    }
// вычисление k-ой амплитуды
double amplitude(int k){
// число интервалов
int nm = u.size();
// длина одного интервал
double dt = period/nm;
if (k == 0){
    double sum = 0;
    for (int n = 1; n <= nm; n++)
    sum+= u[n-1];
    return sum*dt/period;
    }
else {
    double omega = 2*pi/period;
    double sum = 0;
    for (int n = 1; n <= nm; n++)
        sum+= u[n-1]*sin(k*omega*n*dt);
    double Sk = 2*sum*dt/period;
    sum = 0;
    for (int n = 1; n <= nm; n++)
        sum+= u[n-1]*cos(k*omega*n*dt);
    double Ck = 2*sum*dt/period;
    return sqrt(Sk*Sk+Ck*Ck);
    }
}

// построение графика на экране

void paintEvent(QPaintEvent *e){
    if (U.size() == 0) return;
    // положение начала координат графика на экране
    int x0 = 30;
    int y0 = h-30;
    // размер графика
    int width = w-60;
    int height = h-200;
    // определение наибольшей амплитуды
    double Um = 0;
    for (int k = 0; k <= km; k++)
        if (U[k] > Um) Um = U[k];
            QPainter p(this);
    QPen q;
    // оси координат
    q.setWidth(3);
    q.setColor("#000000");
    p.setPen(q);
    p.drawLine(x0,y0,x0+width,y0);
    p.drawLine(x0,y0,x0,y0-height);
    for (int k = 0; k <= km; k++){
        int dx = width/km;
        int x = x0+k*dx;
        int y = (int)(height*U[k]/Um);
    // подписи на оси t
    if (k % 5 == 0)
        p.drawText(x,y0+15,QString::number(k));
    // амплитуды
    p.fillRect(x+1,y0-1,(2*dx)/3,-y,
    QBrush("#ff0000"));
    }
    // подписи на оси амплитуд
    p.drawText(x0-15,y0-height+10,
    QString::number(round(Um)));
    }
}