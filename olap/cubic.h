/***************************************************************************
 *   Copyright (C) 2010 by В.Олоничев                                      *
 *   basv0@mail.ru                                                     *
 *   Copyright (C) 2010 by А.Сенов                                         *
 *   andrey.senov@gmail.com                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QObject>
#include <QMap>
#include <QVector>
#include <QString>
#include <QStringList>

class Ttl: public QObject {
    Q_OBJECT
public:
    Ttl(QString, QObject *parent=0);
    QString getName();
    void setR(double R);
    virtual void updR(double E)=0;
    virtual double getR(int N)=0;
protected:
    double r;
    QString name;
};

class Sum: public Ttl{
    Q_OBJECT
public:
    Sum(QString, QObject *parent=0);
    virtual void updR(double E);
    virtual double getR(int N);
};

class Avg: public Sum{
    Q_OBJECT
public:
    Avg(QString, QObject *parent=0);
    virtual double getR(int N);
};

class Max: public Sum{
    Q_OBJECT
public:
    Max(QString, QObject *parent=0);
    virtual void updR(double E);
};

class Min: public Sum{
    Q_OBJECT
public:
    Min(QString, QObject *parent=0);
    virtual void updR(double E);
};


typedef QVector<QString> s_keys;
typedef QVector<int> i_dims;
typedef QMap<QString,int> m_keys;

struct l_cube{
    s_keys dims;
    double r;
};

typedef QVector<l_cube> data_cube;
typedef QVector<m_keys> vm_keys;
typedef QVector<double> axes_X;
typedef QVector<axes_X> data_plain;

class hyper_cube{
protected:
    l_cube line;
    data_cube dc;               //гиперкуб с данными
    vm_keys cX_keys, cY_keys;   //массивы словарей с частичными ключами
    char dlm;                   //разделитель
    int N;                      //к-во осей гиперкуба
public:
    m_keys *pX, *pY;            //словари с полными ключами
    int Nx, Ny;                 //размер проекции
    int Hx, Hy;                 //к-во осей гиперкуба на осях проекции
    data_plain dp;              //проекция и итоги
    hyper_cube(int n);
    hyper_cube();
    ~hyper_cube();
    void setN(int n);
    int make_proj(i_dims Y, i_dims X, Ttl* pTtl);
    void add(s_keys sk, double res);
    void setData(const data_cube &d, int n);
    //protected:
    void clear();
    void dims(i_dims Y, i_dims X);
};
