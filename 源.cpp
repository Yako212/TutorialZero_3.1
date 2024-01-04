#include<graphics.h>
#include<string>
#include<vector>
#include<math.h>

//动画下标
int idx_current_anim = 0;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

//动画数量
const int PLAYER_ANIM_NUM = 6;

//定义人物动画图片
IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

//初始玩家位置
POINT player_position = { 500,500 };


#pragma comment(lib,"MSIMG32.LIB")
//绘制并显示图像（混合透明）
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

////加载玩家动画-初始版
//void LoadAnimation()
//{
//	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
//	{
//		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
//		loadimage(&img_player_left[i], path.c_str());
//	}
//	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
//	{
//		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
//		loadimage(&img_player_right[i], path.c_str());
//	}
//}

//动画加载-类
class Animation
{
public:
	Animation(LPCTSTR path, int num, int interval)
	{
		interval_ms = interval;

		TCHAR path_file[256];
		for (int i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}

	void play(int x, int y, int delta)
	{
		timer += delta;
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, frame_list[idx_frame]);
	}

	~Animation()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
		{
			delete frame_list[i];
		}
	}
private:
	int timer = 0;			//动画计时器
	int idx_frame = 0;		//动画帧索引
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;
};

class Player
{
public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_right = new Animation(_T("img/1月4日(%d).png"), 13, 180);
		anim_left = new Animation(_T("img/1月4日(%d).png"), 13,180);
	}

	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;
			}
			break;

			//按键状态判断
		case WM_KEYUP:
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = false;
				break;
			case VK_DOWN:
				is_move_down = false;
				break;
			case VK_LEFT:
				is_move_left = false;
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			}
			break;
		}
	}

	void Move()
	{
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		//人物移动的单位距离
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		//人物位置以单位速度移动
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}

		//限制人物在窗口内移动
		if (position.x < 0) position.x = 0;
		if (position.y < 0) position.y = 0;
		if (position.x + PLAYER_WIDTH > WINDOW_WIDTH) position.x = WINDOW_WIDTH - PLAYER_WIDTH;
		if (position.y + PLAYER_HEIGHT > WINDOW_WIDTH) position.y = WINDOW_HEIGHT - PLAYER_HEIGHT;

	}

	void Draw(int delta)
	{
		int pos_shadow_x = position.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + PLAYER_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

		if (facing_left)
		{
			anim_left->play(position.x, position.y, delta);
		}
		else
		{
			anim_right->play(position.x, position.y, delta);
		}
	}

	const POINT& GetPosition() const
	{
		return position;
	}

private:
	const int SPEED = 5;			//玩家移动速度
	const int PLAYER_WIDTH = 80;	//玩家宽度	
	const int PLAYER_HEIGHT = 80;	//玩家高度
	const int SHADOW_WIDTH = 32;	//阴影宽度

	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 600,300 };
	//储存判断按键状态的变量
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};

class Bullet
{
public:
	POINT position = { 0,0 };
public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setlinecolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}

private:
	const int RADIUS = 10;
};

class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);

		//敌人生成边界
		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};
		//将敌人放置在地图外边界处随机的随机位置
		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge)
		{
		case SpawnEdge::Up:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -ENEMY_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -ENEMY_HEIGHT;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		default:
			break;
			
		}
	}
	
	bool CheckBulletCollision(const Bullet& bullet)
	{
		//将子弹等效为点，判断点是否在敌人矩形内
		return false;

	}
	
	bool CheckPlayerCollision(const Player& Player)
	{
		return false;
	}

	void Move(const Player& player)
	{
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
		
	}

	void Draw(int delta)
	{
		int pos_shadow_x = position.x + (ENEMY_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + ENEMY_HEIGHT - 32;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		if (facing_left)
			anim_left->play(position.x, position.y, delta);
		else
			anim_right->play(position.x, position.y, delta);
	}

	~Enemy()
	{
		delete anim_left;
		delete anim_right;
	}
private:
	const int SPEED = 2;			//敌人移动速度
	const int ENEMY_WIDTH = 80;		//敌人宽度	
	const int ENEMY_HEIGHT = 80;	//敌人高度
	const int SHADOW_WIDTH = 48;	//阴影宽度

private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0,0 };
	bool facing_left = false;
};

//生成新的敌人
void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)
		enemy_list.push_back(new Enemy());
}

int main()
{
	//绘制窗口
	initgraph(1280, 720);

	//判断程序运行
	bool running = true;

	//玩家变量
	Player player;
	//存储敌人对象指针
	std::vector<Enemy*> enemy_list;

	//消息变量
	ExMessage msg;

	//背景图片变量
	IMAGE img_backgroud;

	//储存判断按键状态的变量
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;


	//加载背景
	loadimage(&img_backgroud, _T("img/background.png"));
	//loadimage(&img_shadow, _T("img/shadow_player.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();

		//接受消息并处理
		while (peekmessage(&msg))
		{
			player.ProcessEvent(msg);
		}

		//玩家移动
		player.Move();
		//生成新的敌人
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
			enemy->Move(player);

		//清空屏幕
		cleardevice();

		//显示背景图
		putimage(0, 0, &img_backgroud);

		//绘制画面
		player.Draw(1000 / 144);
		for (Enemy* enemy : enemy_list)
			enemy->Draw(1000 / 144);

		FlushBatchDraw();

		//延时帧率处理
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
	}

	EndBatchDraw();

	return 0;
}