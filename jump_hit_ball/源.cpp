#include<easyx.h>
#include<iostream>
#include<vector>
#include<graphics.h>
#include<conio.h>
#include<time.h>
#include<cstringt.h>
#include <stdio.h>
#include <string>
using namespace std;
const auto swidth = 640;
const auto sheight = 800;
const auto hurttime = 1000;
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //xΪ����ͼƬ��X���꣬yΪY����
{

	// ������ʼ��
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
	int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
	int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
	int graphWidth = getwidth();    //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
	int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
	int dstX = 0;    //���Դ������صĽǱ�
	if (picture_x < 0)picture_x = 0;
	if (picture_x > swidth - picture_width)picture_x = swidth - picture_width;
	if (picture_y < 0)picture_y = 0;
	if (picture_y > sheight - picture_height)picture_y = sheight - picture_height;

	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
			int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //���Դ������صĽǱ�
				if (dstX < 0)continue;
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
			}
		}
	}
}//////
bool Rectcrashrect(RECT& r1, RECT& r2)
{
	RECT r;
	r.left = r1.left - (r2.right - r2.left);
	r.right = r1.right;
	r.top = r1.top - (r2.bottom - r2.top);
	r.bottom = r1.bottom;
	return (r.left < r2.left&& r2.left<r.right&& r2.top>r.top&& r2.top < r.bottom);
}
//�ж���ײ
bool pointinrect(int x, int y, RECT& r)
{
	return (x >= r.left && x <= r.right && y >= r.top && y <= r.bottom);
}
class cannon//����
{
public:
	cannon(int x, int y, IMAGE& img)
		:x(x), img(img)
	{
		rect.left = x - img.getwidth() / 2;
		rect.right = x + img.getwidth() / 2;
		rect.top = y - img.getheight();
		rect.bottom = y;
	}
	void show()
	{
		rect.top -= 10;
		rect.bottom -= 10;
		drawAlpha(&img, rect.left, rect.top);
	}
	RECT& getrect() { return rect; }
private:
	RECT rect;
	IMAGE& img;
	int x;
};
class Prop
{
public:
	Prop(int x, IMAGE& img, int type)
		:img(img), type(type)
	{
		rect.left = x - img.getwidth() / 2;
		rect.right = x + img.getwidth() / 2;
		rect.top = 0;
		rect.bottom = img.getheight();
	}
	void show()
	{
		rect.top += 4;
		rect.bottom += 4;
		drawAlpha(&img, rect.left, rect.top);
	}
	RECT& getrect() { return rect; }
	int gettype()
	{
		return type;
	}
private:
	IMAGE& img;
	RECT rect;
	int type;
};
class Ball
{
public:
	Ball(int x, int y, IMAGE& img, int hurt)
		:x(x), y(y), img(img), hurtlast(hurt)
	{
		waittime = 0;
		rect.left = x - img.getwidth() / 2;
		rect.right = x + img.getwidth() / 2;
		rect.top = y - img.getheight() / 2;
		rect.bottom = y + img.getheight() / 2;

		int randnum = rand() % 3;
		//cout <<" " << randnum-1 << endl;
		level = (randnum - 1) * 2;
		ver = 2;


	}
	void hit(int level2)
	{
		if (level == 0)level = level2 * 2;
		ver = -ver;
	}
	int show()
	{
		waittime++;
		int wall = 0;//û����
		if (rect.left == 0 || rect.right == swidth) { level = -level; wall = 1;/*��������*/ }
		if (rect.top == 0 || rect.bottom == sheight) { ver = -ver; wall = 2; /*������*/ }
		rect.left += level;
		rect.right += level;
		rect.top += ver;
		rect.bottom += ver;
		drawAlpha(&img, rect.left, rect.top);
		int randnum = rand() % 100;
		if (randnum > 10 && randnum < 30 && wall)//ȡ��������ʾ����Ƿ�����
		{
			waittime = 0;
			return wall;
		}
		else return 0;
	}
	int gethurtlast()
	{
		return hurtlast;
	}
	void sethurt(int time)
	{
		hurtlast = time;
	}
	int getx() {
		return (rect.left + rect.right) / 2;
	}
	int gety()
	{
		return (rect.top + rect.bottom) / 2;
	}
	void setlevel(int le)
	{
		level = le;
	}
	void setver(int v)
	{
		ver = v;
	}
	int getlevel() {
		return level;
	}
	int getver()
	{
		return ver;
	}

	RECT& getrect() { return rect; }
private:
	int x, y; IMAGE& img; RECT rect;
	int level, ver;//ˮƽ��ֱ
	int hurtlast; int waittime;
};
class Board
{
public:
	Board()
	{
		rect.left = swidth / 2 - 50;
		rect.right = swidth / 2 + 50;
		rect.top = sheight / 8 * 7;
		rect.bottom = rect.top + 2;
		lastmouse = swidth / 2;
		level2 = 0;
		length = 100;
	}
	void show()
	{
		setlinecolor(BLACK);
		setlinestyle(PS_SOLID, 3);
		line(rect.left, rect.top + 1, rect.right, rect.top + 1);
	}
	void control()
	{
		ExMessage mess;
		if (peekmessage(&mess, EM_MOUSE))
		{
			if (mess.x > lastmouse)level2 = 1;
			else if (mess.x < lastmouse)level2 = -1;
			else level2 = 0;
			lastmouse = mess.x;
			rect.left = mess.x - length / 2;
			rect.top = sheight / 8 * 7;
			rect.right = mess.x + length / 2;
			rect.bottom = sheight / 8 * 7 + 2;
			if (rect.left < 0) { rect.left = 0; rect.right = length; }
			if (rect.right > swidth) { rect.right = swidth; rect.left = swidth - length; }


		}
	}
	void longer()
	{
		length = 200;
	}
	void reset()
	{
		length = 100;
	}

	RECT& getrect() { return rect; }
	int getlevel() { return level2; }

private:
	RECT rect;
	int level2;
	int lastmouse;
	int length;
};
void erase3(vector<Ball*>& balls)
{
	int len = balls.size() / 2;
	auto tbb = balls.begin();
	while (len)
	{
		tbb = balls.erase(tbb);
		len--;
	}
}
void addcannnon(vector<cannon*>& cannons, RECT& r, IMAGE& img)
{
	cannon* tt = new cannon((r.left + r.right) / 2, r.top, img);
	cannons.push_back(tt);
}
void winn()
{
	setbkcolor(WHITE);//���ñ�����ɫ
	cleardevice();
	LPCTSTR over = _T("��ɹ��ˣ�");
	LPCTSTR tip = _T("��Enter����");
	settextstyle(20, 0, _T("Monaco"));
	outtextxy(swidth - textwidth(tip), sheight - textheight(tip), tip);
	settextstyle(80, 0, _T("Monaco"));
	outtextxy(swidth / 2 - textwidth(over) / 2, sheight / 3, over);
	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EM_KEY);
		if (mess.vkcode == 0x0D)
		{
			return;
		}
	}
}
void Over()
{
	setbkcolor(WHITE);//���ñ�����ɫ
	cleardevice();
	LPCTSTR over = _T("��ʧ���ˣ�");
	LPCTSTR tip = _T("��Enter����");
	settextstyle(20, 0, _T("Monaco"));
	outtextxy(swidth - textwidth(tip), sheight - textheight(tip), tip);
	settextstyle(80, 0, _T("Monaco"));
	outtextxy(swidth / 2 - textwidth(over) / 2, sheight / 3, over);
	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EM_KEY);
		if (mess.vkcode == 0x0D)
		{
			return;
		}
	}
}
void play()
{
	setbkcolor(WHITE);
	cleardevice();

	IMAGE ball, cann;
	IMAGE props[5];
	loadimage(&ball, _T("..\\image\\1acd9b8f61573671c917de96ee6a9c7d (2).png"));
	//loadimage(&ball, _T("E:\\kun.png"));
	loadimage(&props[0], _T("..\\image\\redball.png"));//�ָ�Ѫ��
	loadimage(&props[1], _T("..\\image\\yellowball.png"));//����Ѫ������
	loadimage(&props[2], _T("..\\image\\greenball.png"));//�ӳ�����
	loadimage(&props[3], _T("..\\image\\blueball.png"));//��������֮һ��
	loadimage(&props[4], _T("..\\image\\superprop.png"));//������
	loadimage(&cann, _T("..\\image\\bomb.png"));
	srand(time(0));//�����


	vector<Ball*>balls;
	Board b = Board();
	int boom = 0;//��ʼ�����ڵ�����
	int ballcnt = 5;//��ʼ�������
	int HP = 3;//��ʼѪ��
	int HPup = 3;//Ѫ������
	bool isplay = true;
	int being = 0;
	int hittime = 0;//�������
	bool is_shoot = false;//�������Ƿ��ڷ���
	int boardlong = 0;
	bool win = false;
	bool hadcannon = false;
	vector<cannon*>cannons;
	vector<Prop*>prop;
	for (int i = 0; i < 5; i++)//��ʼ��������
	{
		int randnum = rand() * 2;
		Ball* t = new Ball(randnum % swidth, randnum % (sheight / 8 * 7), ball, 0);
		balls.push_back(t);
	}

	while (isplay)
	{

		BeginBatchDraw();
		ballcnt = balls.size();

		being++;
		TCHAR* boomt = new TCHAR[128];
		_stprintf_s(boomt, 128, _T("��������%d"), boom);
		TCHAR* ballcntt = new TCHAR[128];
		_stprintf_s(ballcntt, 128, _T("��������%d"), ballcnt);
		TCHAR* HPt = new TCHAR[128];
		_stprintf_s(HPt, 128, _T("HP��%d/%d"), HP, HPup);
		TCHAR* hitt = new TCHAR[128];
		_stprintf_s(hitt, 128, _T("hit_time��%d"), hittime);
		settextcolor(RED);
		settextstyle(25, 0, _T("Monaco"));//�����ı�
		cleardevice();
		outtextxy(10, sheight - 40, boomt);
		outtextxy(110, sheight - 40, ballcntt);
		outtextxy(230, sheight - 40, HPt);
		outtextxy(320, sheight - 40, hitt);

		if (HP <= 0)isplay = false;//Ѫ��Ϊ0������Ϸ
		if (ballcnt == 0) { win = true; isplay = false; }


		//��ӵ���
		if (hittime >= 10)//�����ܼ�����
		{
			boom++; hadcannon = true;
			hittime = 0;
		}


		if (being % 50 == 0)
		{
			int randnum = rand() % 100;
			if (randnum >= 55 && randnum <= 63 && ballcnt > 8)//������֮һ��
			{
				Prop* tball = new Prop(rand() % (swidth - 20), props[3], 3);
				prop.push_back(tball);
			}
			if (randnum >= 6 && randnum <= 8 && hadcannon/*&& ballcnt>10*/)
			{
				Prop* tball = new Prop(rand() % (swidth - 20), props[4], 4);
				prop.push_back(tball);
			}
			if (randnum >= 9 && randnum <= 17 && HP < HPup)//��Ѫ
			{
				Prop* tball = new Prop(rand() % (swidth - 20), props[0], 0);
				prop.push_back(tball);
			}
			if (randnum >= 18 && randnum <= 27 && HP == 1)//����Ѫ������
			{
				Prop* tball = new Prop(rand() % (swidth - 20), props[1], 1);
				prop.push_back(tball);
			}
			if (randnum >= 28 && randnum <= 37)//�ӳ�����
			{
				Prop* tball = new Prop(rand() % (swidth - 20), props[2], 2);
				prop.push_back(tball);
			}
		}

		for (auto i : prop)//���ߵ��ƶ�
		{
			i->show();
		}

		auto pt = prop.begin();//���ߵ�ʰȡ
		while (pt != prop.end())
		{
			RECT rect = (*pt)->getrect();
			if (Rectcrashrect(rect, b.getrect()))//��������
			{
				//�˴�����ʵ�ֹ���
				int proptype = (*pt)->gettype();

				switch (proptype)
				{
				case 0:
					if (HP < HPup)HP++;
					break;
				case 1:
					HPup++; HP++;
					break;
				case 2:
					b.longer();
					boardlong = 500;
					break;
				case 3:

					erase3(balls);
					break;
				case 4:
					if (boom == 0)boom++;
					else {
						addcannnon(cannons, (b.getrect()), cann);
					}
					break;
				default:break;

				}
				pt = prop.erase(pt);
			}
			else
			{
				if (rect.bottom == sheight)//��������
				{
					pt = prop.erase(pt);
				}
				else pt++;
			}
		}
		Sleep(5);
		flushmessage();
		Sleep(2);
		b.control();
		b.show();
		//����

		if (_kbhit())
		{
			char t = _getch();
			if (t == 0x20 && boom)
			{
				addcannnon(cannons, (b.getrect()), cann);
				boom--;

			}
		}


		for (auto i : cannons)//�ڵķ���
		{
			i->show();
		}

		auto tcan = cannons.begin();//����������ʧ
		while (tcan != cannons.end())
		{

			RECT rtcan = (*tcan)->getrect();
			auto tba = balls.begin();
			while (tba != balls.end())
			{
				RECT rtba = (*tba)->getrect();
				if (Rectcrashrect(rtcan, rtba))
				{
					tba = balls.erase(tba);
				}
				else tba++;
			}
			if (rtcan.top <= 0)tcan = cannons.erase(tcan);
			else tcan++;
		}




		/*�˴��������ӵ��ߵ�ʰȡ��
		δʰȡ����ʧ�������ײ���ʧ��
		Ѫ�����޵��˺�ʰȡ�ָ�������Ч�����ж��Ƿ�ﵽ�����������Ƿ�����Ѫ����
		���Ӽӳ���ʱ�ޣ������޵�ʱ��һ��������ʱ�ı��ߺ�rect�ķ�Χ������board�����������޸ĵĺ�����
		�����ڵ�ʹ�ã���������λ�Ͷ���������������ʧ��
		��������֮һ���ʵ�֣�vector����ǰ����֮һ�������erase��
		С���и��ʷ����ӵ�����ѡ��
		*/

		int len = balls.size();
		for (int i = 0; i < len; i++)
		{
			auto tt = balls[i];
			int type = tt->show();
			if (type)//������
			{
				Ball* tb = new Ball(tt->getx(), tt->gety(), ball, clock());
				if (type == 1)
				{
					tb->setlevel(tt->getlevel());
					tb->setver(-tt->getver());
				}
				else
				{
					tb->setlevel(-tt->getlevel());
					tb->setver(tt->getver());
				}
				balls.push_back(tb);
				ballcnt++;
			}
		}

		if (boardlong != 0)
		{

			boardlong--; TCHAR* boardt = new TCHAR[128];
			_stprintf_s(boardt, 128, _T("��������ʱ�䣺%d"), boardlong);
			outtextxy(450, sheight - 40, boardt);
		}
		else { b.reset(); }





		auto t = balls.begin();
		while (t != balls.end())
		{
			RECT trect = (*t)->getrect();
			RECT brect = b.getrect();
			if (Rectcrashrect(b.getrect(), (*t)->getrect()))//���Ӵ����򣬲��趨�޵�ʱ�䣬�����������ڰ����ϵ����
			{
				if (clock() - (*t)->gethurtlast() >= hurttime)
				{
					(*t)->hit(b.getlevel());
					if (boom == 0)
						hittime++;
					(*t)->sethurt(clock());
				}
			}

			if (trect.top > brect.bottom)//�������������
			{
				if (trect.left == 0 || trect.right == swidth || trect.bottom == sheight)
				{
					t = balls.erase(t); ballcnt--; HP--;
				}
				else t++;
			}
			else t++;
		}
		EndBatchDraw();
	}
	if (win)
		winn();
	else
		Over();//����ѭ����������Ϸ
}
void welcome()
{

	LPCTSTR play = _T("��ʼ��Ϸ");
	LPCTSTR texit = _T("������Ϸ");

	RECT tplayr, texitr;//��ʼ��ͽ�����
	setbkcolor(WHITE);//���ñ�����ɫ
	cleardevice();
	settextstyle(60, 0, _T("Monaco"));//�����ı�
	settextcolor(BLACK);//�����ı���ɫ

	tplayr.left = swidth / 2 - textwidth(play) / 2;
	tplayr.right = tplayr.left + textwidth(play);
	tplayr.top = sheight / 5 * 2;
	tplayr.bottom = tplayr.top + textheight(play);

	texitr.left = swidth / 2 - textwidth(texit) / 2;
	texitr.right = texitr.left + textwidth(texit);
	texitr.top = sheight / 5 * 3;
	texitr.bottom = texitr.top + textheight(texit);

	outtextxy(tplayr.left, tplayr.top, play);
	outtextxy(texitr.left, texitr.top, texit);

	while (true) {

		ExMessage mess;
		ExMessage mess1;
		//getmessage(&mess1, /*EM_MOUSE*/WM_MOUSEMOVE);//������

		//outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 10, title);
		outtextxy(tplayr.left, tplayr.top, play);
		outtextxy(texitr.left, texitr.top, texit);

		if (peekmessage(&mess, EM_MOUSE))
		{
			if (mess.lbutton) {
				if (pointinrect(mess.x, mess.y, tplayr)) {//�����ʼ��Ϸ
					return;
				}
				else if (pointinrect(mess.x, mess.y, texitr)) {//���������Ϸ
					exit(0);//�رճ���
				}
			}
		}
		Sleep(5);



	}
}
int main()
{
	initgraph(swidth, sheight,  EW_NOMINIMIZE);
	while (1)
	{
		welcome();
		play();
	}
}