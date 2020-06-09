#include <iostream>
#include <fstream>
#include <graphics.h>
#include <winbgim.h>
#include <time.h>
#define wxmax 1250
#define wymax 750
using namespace std;

int xx, yy;// coltul din stanga sus
int latura; // latura patratului
int frecv[128]; //retine frecventa de aparitie a fiecarei culori pe tabla de joc
bool selectat[32];  //retine daca a fost selectata sau nu o piesa de pe tabla de joc
int numara_pasi[32], numara_pasi2[32];    //numara cate patrate de aceeasi culoare au fost sterse (util in functia "undo")
int culoare_selectata; //retine culoarea selectata de utilizator in orice moment din executia programului
bool viz[32][32], undo_viz[32][32], generare_viz[32][32], vizsol[32][32];
int verif, c, dificultate;
bool ok=1;
struct Tabla{
    bool piesa;
    int x,y,culoare;
};

Tabla tabla[32][32];
int a[32][32];

struct coordonate
{
    int x,y,color;
    bool ok;
};

struct nod{
    coordonate data;
    int sdata;
    nod* next;
};

nod* h=NULL;
nod* qu = NULL;

void push(int x, int y, bool ok, int color)  //pune pe stiva coordonatele, culoarea curenta si prin variabila ok daca a fost selectat un patrat sau daca a fost sters
{
    nod* n=new nod;
    n->data.x=x;
    n->data.y=y;
    n->data.ok=ok;
    n->data.color=color;
    n->next=h;
    h=n;
}

void pop()
{
    if(h==NULL)
        return;
    else
    {
        nod* q=new nod;
        q=h;
        h=h->next;
        delete(q);
    }

}

int topx()
{
    if(h==NULL)
        return 0;
    return h->data.x;
}

int topy()
{
    if(h==NULL)
        return 0;
    return h->data.y;
}


int topok()
{
    if(h==NULL)
        return 0;
    return h->data.ok;
}

int topcolor()
{
    if(h==NULL)
        return 0;
    return h->data.color;
}

nod* s=NULL;

void spush(int verif) //pune pe stiva numarul de patrate sterse in cadrul functie rupeLantul
{
    nod* t=new nod;
    t->sdata=verif;
    t->next=s;
    s=t;
}

void spop()
{
    if(s==NULL)
        return;
    else
    {
        nod* x=new nod;
        x=s;
        s=s->next;
        delete(x);
    }
}


int stop()
{
    if(s==NULL)
        return 0;
    return s->sdata;
}

void citireMatrice()
{
    ifstream f("piese.in");
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
            f>>tabla[i][j].piesa;
    f.close();
}

void resetare()
{
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
        {
            tabla[i][j].culoare=0;
            viz[i][j]=0;
            undo_viz[i][j]=0;
        }
    for(int i=0;i<32;i++)
    {
        frecv[i]=0;
        numara_pasi[i]=0;
        selectat[i]=0;
    }
    while(h!=NULL)
        pop();
    while(s!=NULL)
        spop();

}

void deseneazaTabla(int &x1, int &y1, int &latura)
{
    int x2, y2;
    y1= wymax / 10;
    y2= wymax * 9 / 10;
    latura = (y2 - y1);
    x1 = (wxmax - latura) / 2;
    x2 = (wxmax - latura) / 2 + latura;
    line(x1, y1, x2, y1);
    line(x1, y1, x1, y2);
    line(x2, y1, x2, y2);
    line(x1, y2, x2, y2);
    for(int i = 1; i < dificultate; i++)
    {
        line(x1, y1 + latura * i / dificultate, x2, y1 + latura * i / dificultate);
        line(x1 + latura * i / dificultate, y1, x1 + latura * i / dificultate, y2);
    }
}

void punePiese()
{
    int latPatrMic = latura / dificultate;
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
        {
            if(tabla[i][j].piesa==1)
            {
                setcolor(BROWN);
                circle(xx + j * latPatrMic + latPatrMic/2, yy + i*latPatrMic + latPatrMic/2, latPatrMic/2 - 10);
                setfillstyle(SOLID_FILL, BROWN);
                floodfill(xx + j * latPatrMic + latPatrMic/2, yy + i*latPatrMic + latPatrMic/2,BROWN);
            }
        }
}

int iaCuloare(int i)
{
    switch(i)
    {
        case 0: return BLUE;
        case 1: return LIGHTBLUE;
        case 2: return MAGENTA;
        case 3: return CYAN;
        case 4: return GREEN;
        case 5: return LIGHTGREEN;
        case 6: return RED;
        case 7: return YELLOW;
        case 8: return LIGHTCYAN;
        case 9: return LIGHTGRAY;
        case 10: return LIGHTMAGENTA;
        case 11: return LIGHTRED;
    }
    return BLUE;
}

void deseneazaOptiunileDeCuloare()
{
    //pune culorile in dreapta;
    int x1,x2,y1,y2;
    x1= 10*wxmax/12;
    y1= wymax / 10;
    x2= x1+latura/dificultate;
    y2= latura + y1;
    rectangle(x1,y1,x2,y2);
    for(int i=0;i<dificultate;i++)
        for(int k=0;k<3;k++)
            circle(x1+latura/(2*dificultate),y1+i*latura/dificultate+latura/(2*dificultate), latura/(2*dificultate)-10+k);
    for(int i=0;i<dificultate;i++)
    {
        setfillstyle(SOLID_FILL, iaCuloare(i));
        if(x1>=10 * wxmax / 12 && x1<=10 * wxmax / 12+latura/dificultate)
            floodfill(x1+latura/(2*dificultate),y1+i*(latura/dificultate)+latura/(2*dificultate), BROWN);
    }
}

void deseneazaMeniu()
{
    int x1, x2, y1, y2;
    x1=xx-2*wxmax/12;
    y1=yy+wymax/20;
    x2=x1+2*latura/8;
    y2=latura/8+y1;
    rectangle(x1, y1, x2-latura/8, y2);   //deseneaza butonul pentru "reset"
    rectangle(x1+latura/8, y1, x2, y2);   //deseneaza butonul pentru "undo"
    for(int i=1;i<=3;i++)
        rectangle(x1,y1+i*latura/4,x2,y2+i*latura/4);   //deseneaza optiunile din meniu (3)
    readimagefile("reset.jpg", x1+1, y1+1, x2-latura/8-1, y2-1);
    readimagefile("undo.jpg", x1+latura/8+1, y1+1, x2-1, y2-1);
    settextstyle(0, 0, 15);
    bgiout<<"Meniu"<<endl;
    outstreamxy(x1+20,y1+latura/4+20);
    settextstyle(0, 0, 14);
    bgiout<<"Restart"<<endl;
    outstreamxy(x1+20,y1+2*latura/4+25);
    settextstyle(0, 0, 15);
    bgiout<<"Exit"<<endl;
    outstreamxy(x1+30,y1+3*latura/4+20);
    //outtextxy(x1 + latura/16, y2 + latura/4 - latura/16, "Meniu");
}

bool verificaDacaECompletat()
{
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
            if(tabla[i][j].culoare==0)
                return false;
    return true;
}

bool eUndoApasat(int x, int y)
{
    int x1 = xx - 2*wxmax/12 + latura/8;
    int x2 = x1 + latura/8;
    int y1 = yy + wymax/20;
    int y2 = y1 + latura/8;
    if(x >= x1 && x <=x2 && y >= y1 && y <= y2)
        return true;
    return false;
}

bool eResetApasat(int x, int y)
{
    int x1 = xx - 2*wxmax/12;
    int x2 = x1 + latura/8;
    int y1 = yy + wymax/20;
    int y2 = y1 + latura/8;
    if(x >= x1 && x <= x2 && y >= y1 && y <= y2)
        return true;
    return false;
}

bool eIntrebareApasat(int x, int y)
{
    if(dificultate != 8)
        return false;
    int x1 = 9 * wxmax / 12 ;
    int y1 = yy + wymax/20 + 3 * latura / 4;
    int x2 = x1 + latura/8 + 10;
    int y2 = y1 + latura / 8 + 3 * latura/4;
    if(x <= x2 && x >= x1 && y >= y1 && y <=y2)
        return true;
    return false;
}

void start();
void arataVictoria();
void afiseazaReguli();
bool eReguliApasat(int x, int y);
void arataStart();
void intraInJoc();
void deseneazaOptiuni();
bool eMeniuApasat(int x, int y);

void undo(int  color)
{
    if(verificaDacaECompletat())
    {
        ok=1;
        setfillstyle(XHATCH_FILL, GREEN);
        floodfill(wxmax/2 + 2*latura/dificultate + 10 , (wymax+latura+2*latura/dificultate)/2 - 5, BROWN);
    }
    int latPatrMic=latura/dificultate;
    int i, j;
    j = (topx() - xx) / latPatrMic + 1;
    i = (topy() - yy) / latPatrMic + 1;
    int stanga = xx + (j-1) * latPatrMic + 1;
    int sus = yy + (i-1) * latPatrMic + 1;
    if(topok())
    {
        if(tabla[i-1][j-1].piesa)
        {
            setfillstyle(SOLID_FILL, BROWN);
            floodfill(stanga + latPatrMic/2,sus + latPatrMic/2,BLACK);
            selectat[topcolor()]=0;
        }
        setfillstyle(SOLID_FILL, BLACK);
        floodfill(stanga,sus ,BROWN);
        frecv[topcolor()]--;
        tabla[i-1][j-1].culoare=0;
        pop();
    }
    else
    {
        int col=topcolor();
        int dreapta = xx + j * latPatrMic - 1;
        int jos = yy + i * latPatrMic - 1;
        setcolor(topcolor());
        for(int k=0;k<=5;k++)
            rectangle(stanga+k,sus+k,dreapta-k,jos-k);
        setfillstyle(SOLID_FILL, topcolor());
        if(tabla[i - 1][j - 1].piesa)
        {
            int ii=i,jj=j;
            floodfill(stanga + latPatrMic/2,sus + latPatrMic/2,BLACK);
            tabla[i-1][j-1].culoare=topcolor();
            selectat[topcolor()]=1;
            while(numara_pasi[topcolor()] && topcolor()==col && frecv[topcolor()]<dificultate )//&& !undo_viz[ii][jj] && !viz[i][j])
            {
                jj = (topx() - xx) / latPatrMic + 1;
                ii = (topy() - yy) / latPatrMic + 1;
                int temp_stanga = xx + (jj-1) * latPatrMic + 1;
                int temp_sus = yy + (ii-1) * latPatrMic + 1;
                int temp_dreapta = xx+ jj * latPatrMic - 1;
                int temp_jos = yy + ii * latPatrMic - 1;
                for(int k=0;k<=5;k++)
                    rectangle(temp_stanga+k,temp_sus+k,temp_dreapta-k,temp_jos-k);
                frecv[topcolor()]++;
                tabla[ii-1][jj-1].culoare=topcolor();
                numara_pasi[topcolor()]--;
                pop();
                i=ii;
                j=jj;
            }
        }
        else
        {
            int jj = (topx() - xx) / latPatrMic + 1;
            int ii = (topy() - yy) / latPatrMic + 1;
            int temp_stanga = xx + (jj-1) * latPatrMic + 1;
            int temp_top = yy + (ii-1) * latPatrMic + 1;
            int temp_dreapta = xx+ jj * latPatrMic - 1;
            int temp_jos = yy + ii * latPatrMic - 1;
            frecv[tabla[ii-1][jj-1].culoare]--;
            if(stop()>1)
            {
                while(numara_pasi2[topcolor()] && topcolor()==col && frecv[topcolor()]<dificultate && stop())
                {
                    s->sdata--;
                    jj = (topx() - xx) / latPatrMic + 1;
                    ii = (topy() - yy) / latPatrMic + 1;
                    temp_stanga = xx + (jj-1) * latPatrMic + 1;
                    temp_top = yy + (ii-1) * latPatrMic + 1;
                    temp_dreapta = xx+ jj * latPatrMic - 1;
                    temp_jos = yy + ii * latPatrMic - 1;
                    for(int k=0;k<=5;k++)
                        rectangle(temp_stanga+k,temp_top+k,temp_dreapta-k,temp_jos-k);
                    frecv[topcolor()]++;
                    tabla[ii-1][jj-1].culoare=topcolor();
                    numara_pasi2[topcolor()]--;
                    pop();
                }
                spop();
            }

            else
            {
                for(int k=0;k<=5;k++)
                    rectangle(temp_stanga+k,temp_top+k,temp_dreapta-k,temp_jos-k);
                frecv[topcolor()]++;
                tabla[ii-1][jj-1].culoare=topcolor();
                numara_pasi2[topcolor()]--;
                pop();
                spop();
            }
        }
        //frecv[topcolor()]++;
        //tabla[i-1][j-1].culoare=topcolor();
        //pop();
    }
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
            undo_viz[i][j]=0;
}

bool eAdiacent(int x, int y, int color)
{
    int latPatrMic=latura/dificultate;
    int j = (x - xx) / latPatrMic;
    int i = (y - yy) / latPatrMic;
    if(tabla[i-1][j].culoare==color || tabla[i][j-1].culoare==color || tabla[i+1][j].culoare==color || tabla[i][j+1].culoare==color)
        return true;
    return false;
}

void stergeAceeasiCuloare(int color)
{
    int latPatrMic=latura/dificultate;
    int stanga,sus;
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
            if(!tabla[i][j].piesa && tabla[i][j].culoare==color)
            {
                numara_pasi[color]++;
                stanga = xx + j * latPatrMic + 1;
                sus = yy + i * latPatrMic + 1;
                setfillstyle(SOLID_FILL, BLACK);
                floodfill(stanga, sus ,BROWN);
                push(stanga,sus,0,color);
                frecv[color]--;
                tabla[i][j].culoare=0;
            }
}

void generare();
bool verificaSolutia();
bool eRestartApasat(int x, int y);
void actiuneEcranReguli();
void selectareDificultate();
bool eExitApasat(int x, int y);
void arataOptiunileDeDificultate();

bool eConectatLaPiesa(int i, int j, int color)
{
    if(tabla[i][j].piesa && tabla[i][j].culoare==color)
        return true;
    else
        viz[i][j]=1;
    if(j>=0 && !tabla[i][j].piesa && tabla[i][j-1].culoare==color && !viz[i][j-1] && eConectatLaPiesa(i, j-1, color))
    {
        for(int k=0;k<12;k++)
            for(int l=0;l<12;l++)
                viz[k][l]=0;
        return true;
    }
    if(j<8 && !tabla[i][j].piesa && tabla[i][j+1].culoare==color && !viz[i][j+1] && eConectatLaPiesa(i, j+1, color))
    {
        for(int k=0;k<12;k++)
            for(int l=0;l<12;l++)
                viz[k][l]=0;
        return true;
    }
    if(i>=0 && !tabla[i][j].piesa && tabla[i-1][j].culoare==color && !viz[i-1][j] && eConectatLaPiesa(i-1, j, color))
    {
        for(int k=0;k<12;k++)
            for(int l=0;l<12;l++)
                viz[k][l]=0;
        return true;
    }
    if(i<8 && !tabla[i][j].piesa && tabla[i+1][j].culoare==color && !viz[i+1][j] && eConectatLaPiesa(i+1, j, color))
    {
        for(int k=0;k<12;k++)
            for(int l=0;l<12;l++)
                viz[k][l]=0;
        return true;
    }
    return false;
}
void rupeLantul(int color)
{
    verif=0;
    int latPatrMic=latura/dificultate;
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
        {
            if(tabla[i][j].culoare==color && !eConectatLaPiesa(i, j, color))
            {
                frecv[tabla[i][j].culoare]--;
                int stanga = xx + j * latPatrMic + 1;
                int sus = yy + i * latPatrMic + 1;
                setfillstyle(SOLID_FILL, BLACK);
                floodfill(stanga,sus ,BROWN);
                push(stanga,sus,0,tabla[i][j].culoare);
                tabla[i][j].culoare=0;
                numara_pasi2[color]++;
                undo_viz[i][j]=1;
                verif++;
            }
        }
    if(verif>0)
        spush(verif+1);
    else spush(0);
}

//AICI ADAUG CE E NOU


struct vect{
    int i,j,val;
};
vect v[12];
struct coordonate2{
    int i,j,x;
};

struct node2{
    coordonate2 data;
    node2* next;
};

node2* hh=NULL;

void push2(int i, int j)  //pune pe stiva coordonatele, culoarea curenta si prin variabila ok daca a fost selectat un patrat sau daca a fost sters
{
    node2* n=new node2;
    n->data.i=i;
    n->data.j=j;
    n->next=hh;
    hh=n;
}

void pop2()
{
    if(hh==NULL)
        return;
    else
    {
        node2* q=new node2;
        q=hh;
        hh=hh->next;
        delete(q);
    }

}

int topi2()
{
    if(hh==NULL)
        return 0;
    return hh->data.i;
}

int topj2()
{
    if(hh==NULL)
        return 0;
    return hh->data.j;
}
void afisareMatrice2()
{
    for(int i=0;i<dificultate;i++)
    {
        for(int j=0;j<dificultate;j++)
            cout<<a[i][j]<<" ";
        cout<<endl;
    }
}
void parcurgereSpiralata2(int m, int n)
{
    int i, k = 0, l = 0, p=0;

    /* k - index de inceput al randului
        m - index de final al randului
        l - index de inceput al coloanei
        n - index de final al coloanei
    */

    while (k < m && l < n) {
        //parcurge primul rand din randurile ramase
        for (i = l; i < n; ++i) {
            if(a[k][i]==1)
            {
                v[p].i=k;
                v[p].j=i;
                p++;
            }

        }
        k++;

        //parcurge ultima coloana din coloanele ramase
        for (i = k; i < m; ++i) {
            if(a[i][n-1]==1)
            {
                v[p].i=i;
                v[p].j=n-1;
                p++;
            }

        }
        n--;

        //parcurge ultimul rand din randurile ramase
        if (k < m) {
            for (i = n - 1; i >= l; --i) {
                if(a[m-1][i]==1)
                {
                    v[p].i=m-1;
                    v[p].j=i;
                    p++;
                }

            }
            m--;
        }

        //parcurge prima coloana din coloanele ramase
        if (l < n) {
            for (i = m - 1; i >= k; --i) {
                if(a[i][l]==1)
                {
                    v[p].i=i;
                    v[p].j=l;
                    p++;
                }

            }
            l++;
        }
    }
}
bool verificaDacaECompletat2()
{
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
            if(a[i][j]==0)
                return false;
    return true;
}
void resetareMatrice2()
{
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
            if(a[i][j]!=1)
                a[i][j]=0;
}
void golireStiva2()
{
    while(hh!=NULL)
    {
        pop2();
    }
}
bool verificaSolutia2()
{
    int nr;
    for(int i = 0; i < dificultate; i++)
    {
        for(int j = 0; j < dificultate; j++)
        {
            nr = 0;
            if(a[i][j] == 1)
            {
                nr = 0;
                if(a[i-1][j] == 1)
                    nr++;
                if(a[i+1][j] == 1)
                    nr++;
                if(a[i][j-1] == 1)
                    nr++;
                if(a[i][j+1] == 1)
                    nr++;
                if(i == 0)
                    nr++;
                else if(i == dificultate - 1)
                    nr++;
                if(j == 0)
                    nr++;
                else if(j == dificultate - 1)
                    nr++;
            }
            if(nr == 4)
                return 0;
        }
    }
    return 1;
}

void generare2()
{
    srand (time(NULL));
    int x, y, counter=0;
    for(int i = 0; i < dificultate; i++)
        for(int j = 0; j < dificultate; j++)
            a[i][j] = 0;
    for(int i = 0; i < dificultate; i++)
    {
        x = rand() % dificultate ;
        y = rand() % dificultate ;
        if(a[x][y] == 0)
            a[x][y] = 1;
        else{
            int x2, y2;

            do{
                x2 = rand() % dificultate;
                y2 = rand() % dificultate;
            }while(a[x2][y2] == 1);
            a[x2][y2] = 1;
        }
    }
    for(int i = 0; i < dificultate; i++)
        for(int j = 0; j < dificultate; j++)
            if(a[i][j] == 1)
                counter ++;
    if(counter < dificultate)
        generare2();
    if(!verificaSolutia2())
        generare2();
    else
        return;
}


void creeazaSolutie(){

    int c=0,x,ii,jj,col,counter,p;
    int ok1,ok2;
    srand(time(NULL));
    //citireMatrice(a);
    setcolor(WHITE);
    settextstyle(3, 0, 4);
    bgiout<<"Loading..."<<endl;
    outstreamxy(wxmax/2 - latura/8 - 10 , (wymax+latura+latura/8)/2 - 10);
    regenerare:
    p=0;
    for(int i=0;i<dificultate;i++)
        for(int j=0;j<dificultate;j++)
            a[i][j]=0;
    generare2();
    for(int i = 0; i < dificultate; i++)
        for(int j = 0; j < dificultate; j++)
            tabla[i][j].piesa = a[i][j];
    parcurgereSpiralata2(dificultate, dificultate);
    //cout<<endl;
    while(!verificaDacaECompletat2())
    {
        again:
        col=2;
        p++;
        if(p>10000)
            goto regenerare;
        //cout<<++p<<endl;
        resetareMatrice2();

            for(int i=0;i<dificultate;i++)
            {
                c=1;
                x=0;
                golireStiva2();
                ok1=ok2=2*dificultate+4;
                    counter=0;
                    ii=v[i].i;
                    jj=v[i].j;
                    v[i].val=col;
                    //cout<<ii<<" "<<jj<<endl;
                    while(c<dificultate)
                    {
                        if((jj<=v[0].j+col-2 || jj>=dificultate-v[0].j-col+1) && ii>v[0].i+col-2 && ii<dificultate-v[0].i-col+1 && ok1)
                        {
                            x=rand()%2+2;
                            ok1--;
                        }
                        if((ii<=v[0].i+col-2 || ii>=dificultate-v[0].i-col+1) && jj>v[0].j+col-2 && jj<dificultate-v[0].j-col+1 && ok2)
                        {
                            x=rand()%2;
                            ok2--;
                        }
                        else
                        {
                            x=rand()%4;
                            ok1=ok2=2*dificultate+4;
                        }

                        //cout<<x<<" "<<c<<" "<<counter<<endl;
                        //afisareMatrice(a);
                        //Sleep(700);
                        if(jj>0 && a[ii][jj-1]==0 && x==0)
                        {
                                a[ii][jj-1]=col;
                                jj--;
                                c++;
                                push2(ii,jj);
                                counter++;
                        }
                        else if(jj<dificultate-1 && a[ii][jj+1]==0 && x==1)
                        {
                                a[ii][jj+1]=col;
                                jj++;
                                c++;
                                push2(ii,jj);
                                counter++;
                        }
                        else if(ii>0 && a[ii-1][jj]==0 && x==2)
                        {
                                a[ii-1][jj]=col;
                                ii--;
                                c++;
                                push2(ii,jj);
                                counter++;
                        }
                        else if(ii<dificultate-1 && a[ii+1][jj]==0 && x==3)
                        {
                                a[ii+1][jj]=col;
                                ii++;
                                c++;
                                push2(ii,jj);
                                counter++;
                        }
                        else if((a[ii][jj-1]!=0 || jj<=0) && (a[ii][jj+1]!=0 || jj>=dificultate-1) && (a[ii-1][jj]!=0 || ii<=0) && (a[ii+1][jj]!=0 || ii>=dificultate-1))
                        {
                            if((ii==v[i].i && jj==v[i].j) || counter==0)
                                goto again;
                            counter--;
                            ii=topi2();
                            jj=topj2();
                            pop2();
                        }

                    }
                    col++;


            }
            /*
            cout<<endl;
            for(int i=0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                    cout<<a[i][j]<<" ";
                cout<<endl;
            }*/

    }
    for(int i=0;i<dificultate;i++)
        a[v[i].i][v[i].j]=v[i].val;
    cout<<endl;
    afisareMatrice2();
    golireStiva2();
}
void afiseazaSolutiaCalculatorului()
{
    int stanga, sus, dreapta, jos, p=0, culoare;//, z, c, i, j;
    int latPatrMic=latura/dificultate;
    for(int i=0;i<dificultate;i++)
    {
        sus = yy + i * latPatrMic + 1;
        jos = yy + (i+1) * latPatrMic - 1;
        for(int j=0;j<dificultate;j++)
        {
            stanga = xx + j * latPatrMic + 1;
            dreapta = xx + (j+1) * latPatrMic - 1;
            culoare=iaCuloare(a[i][j]-2);
            setcolor(culoare);
            for(int k=0;k<=5;k++)
                rectangle(stanga+k,sus+k,dreapta-k,jos-k);
            frecv[culoare]++;
            tabla[i][j].culoare=culoare;
            if(tabla[i][j].piesa)
            {
                selectat[culoare]++;
                setfillstyle(SOLID_FILL, culoare);
                floodfill(stanga + latPatrMic/2,sus + latPatrMic/2,BLACK);
            }
        }
    }
}
int main()
{
    //citireMatrice(tabla);
    initwindow(wxmax,wymax, "Impartirea careului");
    arataStart();
    system("pause");
    return 0;
}
void arataVictoria()
{
    bgiout<<"Ai castigat!"<<endl;
    settextstyle(3, 0, 6);
    outstreamxy(wxmax/2-latura/4+latura/32 , (wymax+latura+latura/16)/2-5);
}
void generare()
{
    srand (time(NULL));
    int x, y, counter=0;
    for(int i = 0; i < dificultate; i++)
        for(int j = 0; j < dificultate; j++)
            tabla[i][j].piesa = 0;
    for(int i = 0; i < dificultate; i++)
    {
        x = rand() % dificultate ;
        y = rand() % dificultate ;
        if(tabla[x][y].piesa == 0)
            tabla[x][y].piesa = 1;
        else{
            int x2, y2;

            do{
                x2 = rand() % dificultate;
                y2 = rand() % dificultate;
            }while(tabla[x2][y2].piesa == 1);
            tabla[x2][y2].piesa = 1;
        }
    }
    for(int i = 0; i < dificultate; i++)
        for(int j = 0; j < dificultate; j++)
            if(tabla[i][j].piesa == 1)
                counter ++;
    if(counter < dificultate)
        generare();
    if(!verificaSolutia())
        generare();
    else
        return;
}
bool verificaSolutia()
{
    int nr;
    for(int i = 0; i < dificultate; i++)
    {
        for(int j = 0; j < dificultate; j++)
        {
            nr = 0;
            if(tabla[i][j].piesa == 1)
            {
                nr = 0;
                if(tabla[i-1][j].piesa == 1)
                    nr++;
                if(tabla[i+1][j].piesa == 1)
                    nr++;
                if(tabla[i][j-1].piesa == 1)
                    nr++;
                if(tabla[i][j+1].piesa == 1)
                    nr++;
                if(i == 0)
                    nr++;
                else if(i == dificultate - 1)
                    nr++;
                if(j == 0)
                    nr++;
                else if(j == dificultate - 1)
                    nr++;
            }
            c=0;
            if(nr == 4)
                return 0;
        }
    }
    return 1;
}


void afiseazaReguli()
{
    settextstyle(10, 0, 1);
    setcolor(WHITE);
    moveto(0, 0);
    outtext((char*)"Impartirea Careurilor");
    moveto(0,30);
    outtext((char*)"Scop: ");
    moveto(0,45);
    outtext((char*)"Formati grupe de cate 8 patrate alaturate care sa contina o singura bulina.");
    moveto(0,60);
    outtext((char*)"La final trebuie sa aveti o singura bulina in una din aceste 8 suprafete.");
    moveto(0,90);
    outtext((char*)"Cum se joaca: ");
    moveto(0,105);
    outtext((char*)"Selectati o culoare si incepeti cu una dintre bulinele aflate pe tabla. ");
    moveto(0,120);
    outtext((char*)"Colorati patratele in jurul bulinei astfel incat sa ajungeti la 8/10/12 patrate colorate.");
    moveto(0,150);
    outtext((char*)"Click-Stanga: coloreaza o bulina sau un patrat gol.");
    moveto(0,180);
    outtext((char*)"Click-Dreapta: sterge o colorare a unei buline sau a unui patrat.");
    moveto(0,210);
    outtext((char*)"Undo: reface miscarea de dinainte.");
    moveto(0,240);
    outtext((char*)"Reset: reseteaza tabla fara a schimba pozitiile pieselor.");
    moveto(0,270);
    outtext((char*)"Restart: reseteaza tabla.");
    moveto(0,300);
    outtext((char*)"Pentru nivelul de dificultate 'usor' este posibila rezolvarea automata de catre calculator.");
    moveto(0, 330);
    outtext((char*)"Apasati pe semnul intrebarii pentru a activa.");
    moveto(0,360);
    outtext((char*)"Exit: iesire.");
    moveto(0,390);
    outtext((char*)"Apasati oriune pe ecran pentru a va intoarce la meniu.");
    //system("pause");
}
void deseneazaOptiuni()
{
    int laturabuton = wymax/9;
    int x1 = wxmax / 3 ;
    int y1 = wymax / 3 ;
    int x2 = 2 * wxmax / 3 ;
    int y2 = wymax / 3 + laturabuton;
    for(int i = 0; i < 3; i++)
    {
        setcolor(BROWN);
        rectangle(x1, y1 + 2 * i * laturabuton, x2, y2 + 2 * i * laturabuton);
        setfillstyle(SOLID_FILL, BLACK);
        floodfill(x1 + 1,y1 + 2 * i * laturabuton + 1,BROWN);
    }
    settextstyle(1, 0, 8);
    bgiout<<"Impartirea careului"<<endl;
    outstreamxy(x1/4, y1/3);
    settextstyle(0, 0, 8);
    bgiout<<"Start"<<endl;
    outstreamxy(x1+40, y1+10);
    bgiout<<"Reguli"<<endl;
    outstreamxy(x1+15, y1 + 2*laturabuton + 10);
    bgiout<<"Exit"<<endl;
    outstreamxy(x1+75, y1 + 4*laturabuton + 10);
}
bool eExitApasat(int x, int y)
{
    int x1 = wxmax/3;
    int y1 = 2*wymax/3 + wymax/9;
    int x2 = 2 * wxmax / 3;
    int y2 = y1 + wymax/9;
    if(x >= x1 && x <= x2 && y <= y2 && y >=y1)
        return true;
    return false;
}
bool eReguliApasat(int x, int y)
{
    int x1 = wxmax/3;
    int y1 = 2*wymax/3-wymax/9;
    int x2 = 2 * wxmax / 3;
    int y2 = y1 + wymax/9;
    if(x >= x1 && x <= x2 && y <= y2 && y >=y1)
        return true;
    return false;
}
bool eStartApasat(int x, int y)
{
    int x1 = wxmax/3;
    int y1 = wymax/3;
    int x2 = 2 * wxmax / 3;
    int y2 = wymax / 3 + wymax/9;
    if(x >= x1 && x <= x2 && y <= y2 && y >=y1)
        return true;
    return false;
}
bool eMeniuApasat(int x, int y)
{
    int x1 = xx-2*wxmax/12;
    int y1 = yy+wymax/20 + latura/4;
    int x2 = x1 +2 * latura/8;
    int y2 = latura/8+y1;
    if(x >= x1 && x <= x2 && y <= y2 && y>=y1)
        return true;
    return false;
}
bool eRestartApasat(int x, int y)
{
    int x1 = xx - 2*wxmax/12;
    int x2 = x1 + 2* latura/8;
    int y1 = yy + wymax/20;
    int y2 = y1 + latura/8;
    y1 = y1 + latura/2;
    y2 = y2 + latura/2;
    if(x >= x1 && x <= x2 && y <= y2 && y >= y1)
        return 1;
    return 0;
}
bool eExitDinJocApasat(int x, int y)
{
    int x1 = xx-2*wxmax/12;
    int y1 = yy+wymax/20 + latura/2 + latura/4;
    int x2 = x1 +2 * latura/8;
    int y2 = latura/8+y1;
    if(x >= x1 && x <= x2 && y <= y2 && y>=y1)
        return true;
    return false;
}
void restarteazaTabla()
{
    int stanga, sus;
    int latPatrMic=latura/dificultate;
    int x1,y1;
    x1= 10*wxmax/12;
    y1= wymax / 10;
    for(int i=0;i<dificultate;i++)
    {
        for(int j=0;j<dificultate;j++)
        {
            stanga = xx + j * latPatrMic;
            sus = yy + i * latPatrMic;
            if(tabla[i][j].piesa)
            {
                setfillstyle(SOLID_FILL, 0);
                floodfill(stanga+latPatrMic/2, sus+latPatrMic/2, 0);
            }
            setfillstyle(SOLID_FILL, 0);
            floodfill(stanga+1, sus+1, BROWN);
        }
        for(int k=0;k<3;k++)
            circle(x1+latura/(2*dificultate),y1+i*latura/dificultate+latura/(2*dificultate), latura/(2*dificultate)-10+k);
    }
}
void reseteazaTabla()
{
    int stanga, sus;
    int latPatrMic=latura/dificultate;
    int x1,y1;
    x1= 10*wxmax/12;
    y1= wymax / 10;
    for(int i=0;i<dificultate;i++)
    {
        for(int j=0;j<dificultate;j++)
        {
            stanga = xx + j * latPatrMic;
            sus = yy + i * latPatrMic;
            if(tabla[i][j].piesa)
            {
                setfillstyle(SOLID_FILL, BROWN);
                floodfill(stanga+latPatrMic/2, sus+latPatrMic/2, 0);
            }
            setfillstyle(SOLID_FILL, 0);
            floodfill(stanga+1, sus+1, BROWN);
        }
        for(int k=0;k<3;k++)
            circle(x1+latura/(2*dificultate),y1+i*latura/dificultate+latura/(2*dificultate), latura/(2*dificultate)-10+k);
    }
}
void butonJocAutomat()
{
    int x=xx+latura+35;
    int y=yy;
    setcolor(BROWN);
    rectangle(x,y,x+50,y+50);
    settextstyle(0, 0, 15);
    bgiout<<"?"<<endl;
    outstreamxy(x + 15, y + 10);
}
void intraInJoc()
{
    int color;
    int x=0,y=0,n,m;
    int stanga,sus,dreapta,jos;
    int ai;
    setcolor(BROWN);
    deseneazaTabla(xx, yy, latura);
    deseneazaOptiunileDeCuloare();
    deseneazaMeniu();
    if(dificultate==8)
        butonJocAutomat();
    setfillstyle(XHATCH_FILL, GREEN);
    floodfill(0,0, BROWN);
    restart:
    if(x!=0 && y!=0)
    {
        setfillstyle(XHATCH_FILL, GREEN);
        floodfill(wxmax/2 + 2*latura/dificultate + 10 , (wymax+latura+2*latura/dificultate)/2 - 5, BROWN);
        setcolor(BROWN);
        restarteazaTabla();
    }
    if(dificultate ==8)
        creeazaSolutie();
    else
        generare();
    punePiese();
    reset:
    ai=0;
    setfillstyle(XHATCH_FILL, GREEN);
    floodfill(wxmax/2 + 2*latura/dificultate + 10 , (wymax+latura+2*latura/dificultate)/2 - 5, BROWN);
    setcolor(BROWN);
    int latPatrMic = latura / dificultate;
    int c=10*wxmax/12+latPatrMic/2,v=yy+latPatrMic/2;
    if(x!=0 && y!=0 && !eRestartApasat(x,y))
        reseteazaTabla();
    ok=1;
    resetare();
    color=iaCuloare(0); // culoarea presetata e BLUE
    culoare_selectata=iaCuloare(0);
    for(int k=0;k<3;k++)
    {
        setcolor(WHITE);
        circle(c,v,latPatrMic/2-10+k);  //deseneaza un cerc alb in jurul primei culori pentru a arata ca este selectataNULL
    }
    while(true)
    {
        getmouseclick(WM_LBUTTONDOWN,x,y);
        if(x>xx+latura+35 && x<xx+latura+85 && y>yy && y<yy+50 && ai!=1 && dificultate==8)
        {
            ai=1;
            afiseazaSolutiaCalculatorului();
        }
        if(x > 10 * wxmax / 12 && x< 10 * wxmax / 12 + latPatrMic && y> yy && y< yy + latura) //meniul de selectie al culorii
        {
            color = iaCuloare(((y-yy)/latPatrMic));
            culoare_selectata=iaCuloare(((y-yy)/latPatrMic));
            setcolor(WHITE);
            for(int k=0;k<3;k++)
            {
                circle(((x-10*wxmax/12)/latPatrMic)*latPatrMic+10*wxmax/12+latPatrMic/2,
                       ((y-yy)/latPatrMic)*latPatrMic+yy+latPatrMic/2, latPatrMic/2-10+k); //deseneaza un cerc alb in jurul culorii selectate
            }
            setcolor(BROWN);
            if(((y-yy)/latPatrMic)*latPatrMic+yy+latPatrMic/2!=(v-yy)/latPatrMic*latPatrMic+yy+latPatrMic/2) //verific daca am apasat pe aceeasi culoare de 2 la rand
                for(int k=0;k<3;k++)
                    circle(((c-10*wxmax/12)/latPatrMic)*latPatrMic+10*wxmax/12+latPatrMic/2,  //sterge cercul alb de pe pozitia culorii selectate anterior
                       ((v-yy)/latPatrMic)*latPatrMic+yy+latPatrMic/2, latPatrMic/2-10+k);
            c=x;    // retine pozitia x anterioara
            v=y;    // retine pozitia y anterioara
        }
        if(x > xx && x< latura + xx && y> yy && y< latura + yy && tabla[(y - yy) / latPatrMic][(x - xx) / latPatrMic].culoare==0 &&
           frecv[color]<dificultate &&(eAdiacent(x,y,color) || tabla[(y - yy) / latPatrMic][(x - xx) / latPatrMic].piesa) &&
           (tabla[(y - yy) / latPatrMic][(x - xx) / latPatrMic].piesa==0 || !selectat[color]))
        {
            ai++;
            setcolor(culoare_selectata);
            frecv[color]++;
            setlinestyle(SOLID_LINE,0,1);
            int i, j;
            j = (x - xx) / latPatrMic + 1 ;
            i = (y - yy) / latPatrMic + 1;
            stanga = xx + (j-1) * latPatrMic + 1;
            sus = yy + (i-1) * latPatrMic + 1;
            dreapta = xx + j * latPatrMic - 1;
            jos = yy + i * latPatrMic - 1;
            tabla[i-1][j-1].culoare=color;
            for(int i=0;i<=5;i++)
                rectangle(stanga+i,sus+i,dreapta-i,jos-i);
            setfillstyle(SOLID_FILL, culoare_selectata);
            if(tabla[i - 1][j - 1].piesa)
            {
                floodfill(stanga + latPatrMic/2,sus + latPatrMic/2,BLACK);
                selectat[color]=1;
            }
            push(x,y,1,color);
        }
        getmouseclick(WM_RBUTTONDOWN,n,m);
        if(n>= xx && n<= latura + xx && m>= yy && m<= latura + yy && tabla[(m - yy) / latPatrMic][(n - xx) / latPatrMic].culoare)
        {
            ai++;
            if(verificaDacaECompletat())
            {
                ok=1;
                setfillstyle(XHATCH_FILL, GREEN);
                floodfill(wxmax/2 + 2*latura/dificultate + 10 , (wymax+latura+2*latura/dificultate)/2 - 5, BROWN);
            }
            int i, j;
            j = (n - xx) / latPatrMic + 1;
            i = (m - yy) / latPatrMic + 1;
            frecv[tabla[i-1][j-1].culoare]--;
            stanga = xx + (j-1) * latPatrMic + 1;
            sus = yy + (i-1) * latPatrMic + 1;
            if(tabla[i-1][j-1].piesa)
            {
                setfillstyle(SOLID_FILL, BROWN);
                floodfill(stanga + latPatrMic/2,sus + latPatrMic/2,BLACK);
                selectat[tabla[i-1][j-1].culoare]=0;
                stergeAceeasiCuloare(tabla[i-1][j-1].culoare);
                numara_pasi[tabla[i-1][j-1].culoare]++;
            }
            setfillstyle(SOLID_FILL, BLACK);
            floodfill(stanga,sus ,BROWN);
            push(n,m,0,tabla[i-1][j-1].culoare);
            numara_pasi2[tabla[i-1][j-1].culoare]++;
            tabla[i-1][j-1].culoare=0;
            if(!tabla[i-1][j-1].piesa)
                rupeLantul(topcolor());
            undo_viz[i-1][j-1]=1;
        }
        if(eResetApasat(x,y))
        {
            goto reset;
        }

        if(eUndoApasat(x,y) && (topx()!=0 || ai==1))
        {
            if(ai==1)
                goto reset;
            ai--;
            undo(tabla[(x - xx) / latPatrMic][(y - yy) / latPatrMic].culoare);
        }
        if(eMeniuApasat(x,y))
            arataStart();

        if(eRestartApasat(x,y))
            goto restart;

        if(eExitDinJocApasat(x,y))
            exit(0);

        if(verificaDacaECompletat() && ok)
        {
            cout<<"Ai castigat!"<<endl;
            arataVictoria();
            ok=0;
        }

    }
}
void arataStart()
{
    cleardevice();
    setfillstyle(XHATCH_FILL, GREEN);
    floodfill(0,0, BROWN);
    deseneazaOptiuni();
    int x, y, ok = 1;
    while(ok){
        x=0;
        y=0;
        getmouseclick(WM_LBUTTONDOWN, x, y);
        if(eStartApasat(x, y))
        {
            cleardevice();
            selectareDificultate();
            ok = false;
        }
        if(eReguliApasat(x, y))
        {
            cleardevice();
            afiseazaReguli();
            ok = false;
            actiuneEcranReguli();
        }
        if(eExitApasat(x, y))
        {
            exit(0);
            ok=false;
        }
    }
}
void actiuneEcranReguli()
{
    int x=-1, y=-1;
    bool ok=1;
    while(ok)
    {
        getmouseclick(WM_LBUTTONDOWN, x, y);
        if(x!=-1 && y!=-1)
        {
            ok=0;
            arataStart();
        }
    }

}
void selectareDificultate()
{
    arataOptiunileDeDificultate();
    int x, y;
    bool ok=1;
    while(ok)
    {
        getmouseclick(WM_LBUTTONDOWN, x, y);
        if(eStartApasat(x, y))
        {
            dificultate=8;
            cleardevice();
            intraInJoc();
            ok=0;
        }
        if(eReguliApasat(x, y))
        {
            dificultate=10;
            cleardevice();
            intraInJoc();
            ok=0;
        }
        if(eExitApasat(x, y))
        {
            dificultate=12;
            cleardevice();
            intraInJoc();
            ok=0;
        }
    }
}
void arataOptiunileDeDificultate()
{
    int laturabuton = wymax/9;
    int x1 = wxmax / 3 ;
    int y1 = wymax / 3 ;
    int x2 = 2 * wxmax / 3 ;
    int y2 = wymax / 3 + laturabuton;
    setfillstyle(XHATCH_FILL, GREEN);
    floodfill(0,0, BROWN);
    for(int i = 0; i < 3; i++)
    {
        setcolor(BROWN);
        rectangle(x1, y1 + 2 * i * laturabuton, x2, y2 + 2 * i * laturabuton);
        setfillstyle(SOLID_FILL, BLACK);
        floodfill(x1 + 1,y1 + 2 * i * laturabuton + 1,BROWN);
    }
    settextstyle(1, 0, 8);
    bgiout<<"Impartirea careului"<<endl;
    outstreamxy(x1/4, y1/3);
    settextstyle(0, 0, 8);
    bgiout<<"Usor"<<endl;
    outstreamxy(x1+70, y1+10);
    bgiout<<"Mediu"<<endl;
    outstreamxy(x1+45, y1 + 2*laturabuton + 10);
    bgiout<<"Greu"<<endl;
    outstreamxy(x1+75, y1 + 4*laturabuton + 10);
}
