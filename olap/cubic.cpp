#include "cubic.h"

hyper_cube::hyper_cube(int n){
    dlm='\n';
    N=n;
    line.dims.resize(N);
    //pX=pY=0;
    pX = new m_keys;
    pY = new m_keys;
}

hyper_cube::hyper_cube(){
    N=0;
    //pX=pY=0;
    dlm='\n';
    pX = new m_keys;
    pY = new m_keys;
}

hyper_cube::~hyper_cube(){
    clear();
    if(pX) delete pX;
    if(pY) delete pY;
}

void hyper_cube::setN(int n){
    clear();
    N=n;
    line.dims.resize(N);
    //pX = new m_keys;
    //pY = new m_keys;
}

void hyper_cube::clear(){
    dc.clear();
    line.dims.clear();
}

void hyper_cube::add(s_keys sk, double res){
    line.r = res;
    for(int i=0; i<N; i++)line.dims[i]=sk[i];
    dc.push_back(line);
}

void hyper_cube::setData(const data_cube &d, int n)
{
    dc=d;
    N=n;
    line.dims.clear();
    line.dims.resize(N);
}

void hyper_cube::dims(i_dims Y, i_dims X){
    QString xs, ys;
    (*pY).clear();
    (*pX).clear();
    //формирование словарей с полными составными ключами для осей проекции
    for(int i=0; i<dc.size(); i++){
       xs=""; ys="";
       for(int x=0; x<Hx; x++) xs=xs+dc[i].dims[X[x]];
       for(int y=0; y<Hy; y++) ys=ys+dc[i].dims[Y[y]];
       (*pY)[ys]=-1; (*pX)[xs]=-1;
    }
    //нумерация полных составных ключей для осей проекции
    m_keys::iterator it;
    int m,n;
    m=0;
    for(it=(*pY).begin(); it!=(*pY).end(); it++)
         {it.value()=m; m++;}
    n=0;
    for(it=(*pX).begin(); it!=(*pX).end(); it++)
         {it.value()=n; n++;}

    //формирование массивов словарей с частичными ключами и их нумерация
    //номер перезаписывается, пока идут совпадающие частичные ключи
    //таким образом значение в словаре с частичным ключом является
    //последным значением полного ключа, содержащего данный частичный ключ
    cX_keys.resize(Hx-1);
    cY_keys.resize(Hy-1);
    for(int x=0; x<Hx-1; x++) cX_keys[x].clear();
    for(int y=0; y<Hy-1; y++) cY_keys[y].clear();
    QString ms, c_key;
    QStringList sl;
    for(it=(*pY).begin(); it!=(*pY).end(); it++){
        c_key="";
        ms = it.key();
        sl = ms.split(dlm,QString::SkipEmptyParts);
        for(int i=0; i<sl.size()-1; i++){
            c_key=c_key+sl.at(i)+dlm;
            cY_keys[i][c_key]=it.value();
        }
    }

    for(it=(*pX).begin(); it!=(*pX).end(); it++){
        c_key="";
        ms = it.key();
        sl = ms.split(dlm,QString::SkipEmptyParts);
        for(int i=0; i<sl.size()-1; i++){
            c_key=c_key+sl.at(i)+dlm;
            cX_keys[i][c_key]=it.value();
        }
    }
}

int hyper_cube::make_proj(i_dims Y, i_dims X, Ttl* pTtl){
    QString xs, ys;
    int Iy, Jx;
    Hx=X.size();
    Hy=Y.size();
    dims(Y,X);//формирование словарей с полными и частичными ключами

    //Выделение памяти под проекцию и итоги и ее обнуление
    for(int i=0; i<dp.size(); i++) dp[i].clear();
    dp.clear();
    Nx = (*pX).size();
    Ny = (*pY).size();
    dp.resize(Ny+X.size());
    for(int i=0; i<Ny+Hx; i++){
        dp[i].resize(Nx+Hy);
        dp[i].fill(0.0);
    }

    //Заполнение проекции
    for(int i=0; i<dc.size(); i++){
        //воостановление ключей из текущей строчки таблицы с результатами
        xs=""; ys="";
        for(int x=0; x<Hx; x++) xs=xs+dc[i].dims[X[x]];
        for(int y=0; y<Hy; y++) ys=ys+dc[i].dims[Y[y]];

        //индексы клетки проекции для полных ключей текущей строки
        Iy = (*pY)[ys];
        Jx = (*pX)[xs];

        //добавление анализируемой величины к клетке проекции
        dp[Iy][Jx]+=dc[i].r;
    }

    int Ip,Jp;
    m_keys::iterator it;
    //Заполнение итогов
    //итоги по полным ключам
    for(int i=0; i<Ny; i++){
        pTtl->setR(dp[i][0]);
        for(int j=1; j<Nx; j++) pTtl->updR(dp[i][j]);
        dp[i][Nx]=pTtl->getR(Nx);
    }
    for(int j=0; j<Nx; j++){
        pTtl->setR(dp[0][j]);
        for(int i=1; i<Ny; i++) pTtl->updR(dp[i][j]);
        dp[Ny][j]=pTtl->getR(Ny);
    }
    pTtl->setR(dp[Ny][0]);
    for(int j=1; j<Nx; j++) pTtl->updR(dp[Ny][j]);
    dp[Ny][Nx]=pTtl->getR(Nx);
    //Итоги по частичным ключам
    for(int x=0; x < Hx-1; x++){
        Jp=0;
        for(it=cX_keys[x].begin(); it!=cX_keys[x].end(); it++){
            int Jc=it.value();
            pTtl->setR(dp[Ny][Jp]);
            for(int j=Jp+1;j<=Jc;j++) pTtl->updR(dp[Ny][j]);
            dp[Ny+Hx-1-x][Jc]=pTtl->getR(Jc-Jp+1);
            Jp=Jc+1;
        }
    }
    for(int y=0; y < Hy-1; y++){
        Ip=0;
        for(it=cY_keys[y].begin(); it!=cY_keys[y].end(); it++){
            int Ic=it.value();
            pTtl->setR(dp[Ip][Nx]);
            for(int i=Ip+1;i<=Ic;i++)pTtl->updR(dp[i][Nx]);
            dp[Ic][Nx+Hy-1-y]=pTtl->getR(Ic-Ip+1);
            Ip=Ic+1;
        }
    }
    return 0;
}

Ttl::Ttl(QString n, QObject *parent) : QObject(parent){name=n;}
Sum::Sum(QString n, QObject *parent):Ttl(n,parent){}
Avg::Avg(QString n, QObject *parent):Sum(n,parent){}
Max::Max(QString n, QObject *parent):Sum(n,parent){}
Min::Min(QString n, QObject *parent):Sum(n,parent){}
QString Ttl::getName(){return name;}
void Ttl::setR(double R){r=R;}
void Sum::updR(double E){r+=E;}
void Max::updR(double E){if(E>r)r=E;}
void Min::updR(double E){if(E<r)r=E;}
double Sum::getR(int /*N*/){return r;}
double Avg::getR(int N){return r/N;}
