#include "teamstyle17.h"
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<iostream>
#define MAX_SIZE 1000
#define WAIT  while(GetTime()==operate_time);
#define GO   (operate_time=GetTime());
#define NUM_OF_SOLUTION  3
enum value {
	ENERGY_VALUE = 7 * (int)1e6,
	HIGHLY_ADVANCED_VALUE = 7 * (int)1e7,
	MID_ADVANCED_VALUE = 2 * (int)1e7,
	LOW_ADVANCED_VALUE = 8 * (int)1e5,
	LOW_LOW_ADVANCED_VALUE = (int)1e5,
	TRASH = 0,
};
enum situation {
	NONE = 0,
	OPPONENT = 1 << 0,
	SEE_BOSS = 1 << 1,
};
struct point {
	int weight;
	Position pos;
};
struct ball {
	Position center;
	double radius;
};

//double opponent_radius;//			initial()						opponent()
//double boss_radius;//				initial()						greedy()
Position boss_pos;//				initial()						action()
Position opponent_pos;//			initial()						opponent()
PlayerObject me;//					initial()						almost everywhere
int emergency;	//					opponent(),boss()				greedy()
int ad_weight = HIGHLY_ADVANCED_VALUE;//				initial()						initial()
int num_of_aim;//					greedy()						avoid()
int num_of_food;//					initial()						greedy()
int num_of_devour;//				initial()						avoid()
int code;//							initial()						AIMain()&greedy()
double me_radius;//					initial()						almost everywhere
Object boss_obj;//					initial()						action()
Object opponent_obj;//				initial()						opponent()
point aim[MAX_SIZE];//				greedy()						avoid()
point food[MAX_SIZE];//				initial()						greedy()
Position devour[MAX_SIZE];//		initial()						avoid()
ball devour_for_YQY[MAX_SIZE];//	initial()						avoid()
point solution[NUM_OF_SOLUTION];//	opponent(),boss()				avoid()
Position go_for;//					avoid()							move()
Position last_move;//				move()							move()
int operate_time;//					after any action				before any action
bool chasing_boss = false;

//core function						usage							author	time cost
int initial();//					initial,update the basic value	ARC
void greedy();//					find the best food				ZWT
int update();//						update the skills, shield		ZWT		1cost
void avoid();//						avoid the devour and border		YQY
int opponent();//					deal with the opponent			PLU		1cost
int boss();//						smaller, kill; bigger, eat		ARC		1cost		
void move();//						move to							PLU		1cost

			//auxiliary variables
int en_weight = 20;
char bitmap[(MAX_SIZE >> 3) + 1];

//auxiliary function
int zw_cost(int skill);
int zw_cmp(const void*, const void*);
int long_attack(const Object& target);
int short_attack(const Object& target);
int dash();
void zw_enshaw();

//lower level function
Position add(Position a, Position b);//a+b
Position minus(Position a, Position b);//a-b，从B指向A的向量
Position multiple(double k, Position a);//ka，数乘
Position cross_product(Position a, Position b);//a*b 叉乘
double dot_product(Position a, Position b);//ab
double length(Position a);//求矢量模长
Position norm(Position a);//求单位矢量
double distance(Position a, Position b);//求AB两点距离
void show(Position a);//输出矢量 

					  //Main
void AIMain() {
	if (GetStatus()->team_id == 1)return;
	for (;;) {
		srand(time(0));
		code = initial();
		if (distance(GetStatus()->objects[0].pos, me.pos) > 2 * me.radius) goto AVOID;
		update();
		if (distance(GetStatus()->objects[0].pos, me.pos) > 2 * me.radius) goto AVOID;
		if (code&OPPONENT) {
			opponent();
		}
		if (distance(GetStatus()->objects[0].pos, me.pos) > 2 * me.radius) goto AVOID;
		if (code&SEE_BOSS) {
			boss();
		}
		else {
			if (chasing_boss) {
				printf("chasing canceled\n");
			}
			chasing_boss = false;
		}
		if (distance(GetStatus()->objects[0].pos, me.pos) > 2 * me.radius) goto AVOID;
		if (!emergency) {
			greedy();
		}
		if (false) {
		AVOID:
			initial();
		}
		if (me.skill_level[SHIELD] < kMaxSkillLevel || me.shield_time < 10) {
			avoid();
		}
		else {
			go_for = aim[0].pos;
		}
		move();
	}
}
//function body
Position add(Position a, Position b)
{
	Position c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	return c;
}
Position minus(Position a, Position b)
{
	Position c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	return c;
}
Position multiple(double k, Position a)
{
	Position c;
	c.x = k*a.x;
	c.y = k*a.y;
	c.z = k*a.z;
	return c;
}
Position cross_product(Position a, Position b)
{
	Position c;
	c.x = a.y*b.z - a.z*b.y;
	c.y = -a.x*b.z + a.z*b.x;
	c.z = a.x*b.y - a.y*b.x;
	return c;
}
double dot_product(Position a, Position b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}
double length(Position a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}
Position norm(Position a)
{
	double l = 1 / length(a);
	return multiple(l, a);
}
double distance(Position a, Position b)
{
	return length(minus(a, b));
}
void show(Position a)
{
	printf("The position is (%f,%f,%f).\n", a.x, a.y, a.z);
}
int zw_cost(int skill) {
	if (me.skill_level[skill]) {
		return kBasicSkillPrice[skill] << me.skill_level[skill];
	}
	else {
		int count = 0;
		for (int i = 0; i < kSkillTypes; i++) {
			if (me.skill_level[i]) count++;
		}
		return kBasicSkillPrice[skill] << count;
	}
}
int update() {
	if (me.skill_level[SHIELD] >= 3 && me.skill_cd[SHIELD] == 0 && me.short_attack_casting == -1 && me.long_attack_casting == -1) {
		WAIT;
		Shield(me.id);
		GO;
		return 1;
	}
	else {
		if (me.skill_level[VISION_UP] == 0) {
			if (me.ability >= zw_cost(VISION_UP)) {
				WAIT;
				UpgradeSkill(me.id, VISION_UP);
				GO;
				return 1;
			}
			return 0;
		}
		if (me.skill_level[HEALTH_UP] < kMaxSkillLevel) {
			if (me.ability >= zw_cost(HEALTH_UP)) {
				WAIT;
				UpgradeSkill(me.id, HEALTH_UP);
				GO;
				return 1;
			}
			else return 0;
		}
		else if (me.skill_level[SHIELD] < kMaxSkillLevel) {
			if (me.ability >= zw_cost(SHIELD)) {
				WAIT;
				UpgradeSkill(me.id, SHIELD);
				GO;
				return 1;
			}
			else return 0;
		}
		else {
			ad_weight = MID_ADVANCED_VALUE;//1st step of update finish
			if (me.skill_level[SHORT_ATTACK] < kMaxSkillLevel) {
				if (me.skill_level[DASH] < kMaxSkillLevel) {
					if (me.ability >= zw_cost(SHORT_ATTACK)) {
						WAIT;
						UpgradeSkill(me.id, SHORT_ATTACK);
						GO;
						return 1;
					}
					else if (me.ability >= zw_cost(DASH)) {
						WAIT;
						UpgradeSkill(me.id, DASH);
						GO;
						return 1;
					}
					else return 0;
				}
				else {
					if (me.ability >= zw_cost(SHORT_ATTACK)) {
						WAIT;
						UpgradeSkill(me.id, SHORT_ATTACK);
						GO;
						return 1;
					}
					else return 0;
				}
			}
			else if (me.skill_level[DASH] < kMaxSkillLevel) {
				if (me.ability >= zw_cost(DASH)) {
					WAIT;
					UpgradeSkill(me.id, DASH);
					GO;
					return 1;
				}
				else return 0;
			}
			else {
				ad_weight = LOW_ADVANCED_VALUE;//2st step of update finish
				if (me.skill_level[LONG_ATTACK] < kMaxSkillLevel) {
					if (me.ability >= zw_cost(LONG_ATTACK)) {
						WAIT;
						UpgradeSkill(me.id, LONG_ATTACK);
						GO;
						return 1;
					}
					else return 0;
				}
				else {
					//ad_weight = LOW_LOW_ADVANCED_VALUE;
					ad_weight = LOW_ADVANCED_VALUE;
					if (me.skill_level[VISION_UP] < kMaxSkillLevel) {
						if (me.ability >= zw_cost(VISION_UP)) {
							WAIT;
							UpgradeSkill(me.id, VISION_UP);
							GO;
							return 1;
						}
						else {
							return 0;
						}
					}
					else {
						ad_weight = TRASH;
					}

				}
			}
			return 0;
		}
	}
}
void greedy() {
	double check = 1.2*me_radius;
	if (emergency)return;
	zw_enshaw();
	int temp = MAX_SIZE >> 3;
	for (; temp >= 0; --temp) {
		bitmap[temp] = 0;
	}
	while (true) {
		int next;
		for (next = 0; next < num_of_food; next++) {
			if (!(bitmap[next >> 3] & (0x80 >> (next & 0x07)))) break;
		}
		if (next == num_of_food)break;
		aim[num_of_aim] = food[next];
		bitmap[next >> 3] |= (0x80 >> (next & 0x07));
		int i;
		for (i = 0; i < num_of_food; i++) {
			if (!(bitmap[i >> 3] & (0x80 >> (i & 0x07)))) {
				if (distance(aim[num_of_aim].pos, food[i].pos) < check) {
					bitmap[i >> 3] |= (0x80 >> (i & 0x07));
				}
			}
		}
		num_of_aim++;
	}
	for (int temp = num_of_aim - 1; temp>0; --temp) {
		aim[temp].weight /= distance(aim[temp].pos, me.pos);
	}
	//std::cout << "++" << std::endl;
	//for (int temp = num_of_aim - 1; ~temp; --temp) {
	//std::cout << aim[temp].weight << std::endl;
	//}
	//std::cout << "++" << std::endl;
	qsort(aim, num_of_aim, sizeof(point), zw_cmp);
}
int zw_cmp(const void* p, const void* q) {
	int w1 = (((point*)p)->weight);
	int w2 = (((point*)q)->weight);
	return ((w2 - w1) > 0);
}
void avoid()
{
	me = GetStatus()->objects[0];
	Position default_pos[6];
	const Position a[6] = { { 1000,0,0 },{ -1000,0,0 },{ 0,1000,0 },{ 0,-1000,0 },{ 0,0,1000 },{ 0,0,-1000 } };
	default_pos[0] = add(me.pos, a[0]);
	default_pos[1] = add(me.pos, a[1]);
	default_pos[2] = add(me.pos, a[2]);
	default_pos[3] = add(me.pos, a[3]);
	default_pos[4] = add(me.pos, a[4]);
	default_pos[5] = add(me.pos, a[5]);
	int IsDevour(double d, Position des, Position speed);
	Position Schmidt(Position a1, Position a2);
	int flag;//记录是否选取
	int flag2;//记录是否在瞬移后前进方向有devour
	int j, devour_count;
	int devour_danger;
	int i;
	Position aim_devour;
	if (solution[2].weight>aim[0].weight)
	{
		go_for = solution[2].pos;
		printf("see boss\n");
		return;
	}
	if (distance(aim[0].pos, me.pos) / kMaxMoveSpeed <= me.shield_time && me.skill_level[SHIELD] == 5)
	{
		go_for = aim[0].pos;
		printf("hahaha\n");
		return;
	}
	for (i = 0; i<num_of_aim; i++)
	{
		flag = 1;
		for (j = 0; j<num_of_devour; j++)
		{
			if (distance(aim[i].pos, devour[j])<1 * me.radius)//如果目标附近有吞噬者
				if (distance(aim[i].pos, me.pos)> 0.5*distance(devour[j], me.pos))
					flag = 0;
		}
		if (flag == 0)//旁边有devour，扔掉
		{
			printf("throw it away!\n");
			continue;
		}
		else
		{
			Position speed = minus(aim[i].pos, me.pos);
			devour_count = 0;
			if (distance(aim[i].pos, me.pos) / kMaxMoveSpeed <= me.shield_time && me.skill_level[SHIELD] == 5)
			{
				devour_danger = 0;
				printf("hahaha\n");
			}
			else
				devour_danger = 1;
			if (devour_danger)
			{
				for (j = 0; j<num_of_devour; j++)
				{
					if (IsDevour(1.1*me.radius, devour[j], speed))
					{
						devour_count++;
						aim_devour = devour[j];
					}
				}
			}
			if (devour_count >= 2)
				continue;
			else
				if (devour_count == 1)
				{
					printf("warning\n");
					Position a2 = minus(aim_devour, me.pos);
					/*if (IsBorder(1.1*me.radius,add(me.pos, speed)))
					{
					printf("边界哦！\n");
					speed=FBorder(1.1*me.radius,speed);
					}*/
					speed = Schmidt(speed, a2);
					go_for = add(me.pos, speed);
					return;
				}
				else
				{
					go_for = aim[i].pos;
					return;
				}
		}
	}
	if (i == num_of_aim)
	{
		printf("sudden move\n");
		flag2 = 0;
		for (j = 0; j<num_of_devour; j++)
		{
			if (IsDevour(1.5*me.radius, devour[j], me.speed))
				flag2 = 1;
		}
		if (!flag2)//如果没有问题
		{
			go_for = add(me.pos, me.speed);
			return;
		}
		else//有问题，随机一个没有devour的方向
		{
			for (i = 0; i<6; i++)
			{
				for (j = 0; j<num_of_devour; j++)
					if (!IsDevour(1.5*me.radius, devour[j], a[i]))
					{
						go_for = default_pos[i];
						return;
					}
			}
		}
	}
}

Position Schmidt(Position a1, Position a2)
{
	Position temp1, temp2;
	temp2 = multiple(dot_product(a1, a2) / dot_product(a2, a2), a2);
	temp1 = minus(a1, temp2);
	return temp1;
}
int IsDevour(double d, Position des, Position speed)//判断下一时刻会不会碰到吞噬者
{
	Position MaximumSpeed(Position vec);
	int flag = 0;
	Position Next;
	for (int i = 1; i <= 10; i++)
	{
		Next = add(me.pos, multiple(i, MaximumSpeed(speed)));
		if (distance(Next, des)<d)
			flag = 1;
	}
	return flag;
}
Position MaximumSpeed(Position vec) {
	register double len = length(vec);
	vec.x *= (kMaxMoveSpeed + kDashSpeed[me.skill_level[DASH]]) / len;
	vec.y *= (kMaxMoveSpeed + kDashSpeed[me.skill_level[DASH]]) / len;
	vec.z *= (kMaxMoveSpeed + kDashSpeed[me.skill_level[DASH]]) / len;
	return vec;
}
void move() {
	Position speed;
	double mode = (10 + kMaxMoveSpeed + kDashSpeed[me.skill_level[DASH]]) / distance(go_for, me.pos);
	speed = multiple(mode, minus(go_for, me.pos));
	// edge
	//if (me.radius > 8000) {
	const double threshold = 0.05 * me.radius;
	if (me.pos.x - me.radius < threshold && speed.x < 0) {
		speed.x = 0;
	}
	if (me.pos.y - me.radius < threshold && speed.y < 0) {
		speed.y = 0;
	}
	if (me.pos.z - me.radius < threshold && speed.z < 0) {
		speed.z = 0;
	}
	if (kMapSize - me.pos.x - me.radius < threshold && speed.x > 0) {
		speed.x = 0;
	}
	if (kMapSize - me.pos.y - me.radius < threshold && speed.y > 0) {
		speed.y = 0;
	}
	if (kMapSize - me.pos.z - me.radius < threshold && speed.z > 0) {
		speed.z = 0;
	}
	if (length(speed) == 0) {
		printf("center\n");
		mode = (10 + kMaxMoveSpeed + kDashSpeed[me.skill_level[DASH]]) / distance({ kMapSize / 2, kMapSize / 2, kMapSize / 2 }, me.pos);
		speed = multiple(mode, minus({ kMapSize / 2, kMapSize / 2, kMapSize / 2 }, me.pos));
	}
	else {
		speed = multiple((10 + kMaxMoveSpeed + kDashSpeed[me.skill_level[DASH]]), norm(speed));
	}
	//}
	Move(me.id, speed);
	last_move = norm(speed);
}
point mi_zhi_yin_qiu_yang(int n) {
	if (n >= 2) {
		return aim[n - 2];
	}
	else {
		return solution[n];
	}
}
void zw_enshaw() {
	double len = 2 * me.radius;
	double div = 3.5e-7;//?
	num_of_aim = 1;
	Position force = { 0.0,0.0,0.0 };
	int n = num_of_food - 1;
	for (; n >= 0; n--) {
		Position point_to = minus(food[n].pos, me.pos);
		double k = me_radius / length(point_to);
		force = add(force, multiple(k*k*food[n].weight, point_to));
	}
	aim[0].weight = (int)((length(force))*div);
	aim[0].pos = add(me.pos, force);
}
int initial() {
	me = GetStatus()->objects[0];
	num_of_aim = num_of_food = num_of_devour = 0;
	emergency = code = 0;
	me_radius = me.radius;
	const Map *map = GetMap();
	const double ratio = 1.1;
	int i = (*map).objects_number - 1;
	for (; ~i; --i) {
		switch ((*map).objects[i].type) {
		case PLAYER:
			if ((*map).objects[i].team_id == GetStatus()->team_id) break;
			opponent_obj = (*map).objects[i];
			code ^= OPPONENT;
			break;
		case ENERGY:
			if ((*map).objects[i].pos.x - ratio * me_radius < 0) break;
			if ((*map).objects[i].pos.y - ratio * me_radius < 0) break;
			if ((*map).objects[i].pos.z - ratio * me_radius < 0) break;
			if ((*map).objects[i].pos.x + ratio * me_radius > kMapSize) break;
			if ((*map).objects[i].pos.y + ratio * me_radius > kMapSize) break;
			if ((*map).objects[i].pos.z + ratio * me_radius > kMapSize) break;
			food[num_of_food].weight = ENERGY_VALUE;
			food[num_of_food].pos = (*map).objects[i].pos;
			++num_of_food;
			break;
		case ADVANCED_ENERGY:
			if (distance((*map).objects[i].pos, { 0, 0, 0 }) < ratio * me_radius) break;
			if (distance((*map).objects[i].pos, { 0, 0, kMapSize }) < ratio * me_radius) break;
			if (distance((*map).objects[i].pos, { 0, kMapSize, 0 }) < ratio * me_radius) break;
			if (distance((*map).objects[i].pos, { 0, kMapSize, kMapSize }) < ratio * me_radius) break;
			if (distance((*map).objects[i].pos, { kMapSize, 0, 0 }) < ratio * me_radius) break;
			if (distance((*map).objects[i].pos, { kMapSize, 0, kMapSize }) < ratio * me_radius) break;
			if (distance((*map).objects[i].pos, { kMapSize, kMapSize, 0 }) < ratio * me_radius) break;
			if (distance((*map).objects[i].pos, { kMapSize, kMapSize, kMapSize }) < ratio * me_radius) break;
			else {
				//if (!chasing_boss) {
				//	food[num_of_food].weight = ad_weight;
				//}
				//else {
				//	food[num_of_food].weight = TRASH;
				//}
				food[num_of_food].weight = ad_weight;
				food[num_of_food].pos = (*map).objects[i].pos;
				++num_of_food;
			}
			break;
		case SOURCE:
			break;
		case DEVOUR:
			devour[num_of_devour] = (*map).objects[i].pos;
			++num_of_devour;
			break;
		case BOSS:
			boss_obj = (*map).objects[i];
			code ^= SEE_BOSS;
			break;
		default:
			break;
		}
	}
	return code;
}
int boss() {
	if (boss_obj.radius < me_radius * kEatableRatio) {
		solution[SEE_BOSS].weight = 1e7;
		solution[SEE_BOSS].pos = boss_obj.pos;
		printf("chasing boss: %f, %f\n", boss_obj.radius, me_radius);
		chasing_boss = true;
		return 0;
	}
	else {
		if (chasing_boss) {
			printf("chasing canceled\n");
		}
		chasing_boss = false;
		const double safe_distance = 500;
		emergency = 0;
		if (me_radius < boss_obj.radius * kEatableRatio &&
			distance(boss_obj.pos, me.pos) - boss_obj.radius < safe_distance) {
			emergency = 1;
		}
		if (emergency) {
			printf("boss emergency\n");
			solution[SEE_BOSS].weight = 1e6;
			solution[SEE_BOSS].pos = add(me.pos, minus(me.pos, boss_obj.pos));
		}
		//int tmp = short_attack(boss_obj);
		//if (!~tmp) tmp = long_attack(boss_obj);
		//return (~tmp) ? 1 : 0;
		return 0;
	}
}
int long_attack(const Object& target)
{
	dash();
	if (me.skill_cd[LONG_ATTACK] == -1) {
		return -1;
	}
	if (me.long_attack_casting != -1 || me.short_attack_casting != -1) {
		return -1;
	}
	if (target.shield_time > 0) {
		return -1;
	}
	if (distance(target.pos, me.pos) - target.radius - me.radius
		> kLongAttackRange[me.skill_level[LONG_ATTACK]]) {
		return -1;
	}
	WAIT;
	LongAttack(me.id, target.id);
	GO;
	return 0;
}

int short_attack(const Object& target)
{
	dash();
	if (me.skill_cd[SHORT_ATTACK] == -1) {
		return -1;
	}
	if (me.long_attack_casting != -1 || me.short_attack_casting != -1) {
		return -1;
	}
	if (target.shield_time > 0) {
		return -1;
	}
	if (distance(target.pos, me.pos) - target.radius - me.radius
			> kShortAttackRange[me.skill_level[SHORT_ATTACK]]) {
		return -1;
	}
	WAIT;
	ShortAttack(me.id);
	GO;
	return 0;
}

int dash()
{
	if (me.long_attack_casting != -1 || me.short_attack_casting != -1) {
		return -1;
	}
	if (!me.skill_level[DASH]) {
		return -1;
	}
	if (me.skill_cd[DASH] == -1) {
		return -1;
	}
	WAIT;
	Dash(me.id);
	GO;
	return 0;
}

int opponent()
{
	const double safe_distance = 1000;
	int result = 0;
	solution[OPPONENT].pos = { kMapSize / 2, kMapSize / 2, kMapSize / 2 };
	solution[OPPONENT].weight = 0;
	if (short_attack(opponent_obj) == 0 || long_attack(opponent_obj) == 0) {
		result = 1;
	}
	/*if (opponent_obj.radius > me.radius && me.skill_level[SHORT_ATTACK] == 0) {
	solution[OPPONENT].pos = add(me.pos, minus(me.pos, opponent_obj.pos));
	solution[OPPONENT].weight = 1e8;
	}*/
	if (opponent_obj.radius > me.radius / (kEatableRatio * 1.05)) {
		solution[OPPONENT].pos = add(me.pos, minus(me.pos, opponent_obj.pos));
		if (distance(opponent_obj.pos, me.pos) < kVision[0]) {
			solution[OPPONENT].weight = 1e9;
		}
		else {
			solution[OPPONENT].weight = 1e4;
		}
		if (distance(me.pos, opponent_obj.pos) - opponent_obj.radius < safe_distance) {
			emergency = 1;
		}
	}
	else if (opponent_obj.radius < me.radius * kEatableRatio
		&& distance(me.pos, opponent_obj.pos) - opponent_obj.radius < safe_distance) {
		solution[OPPONENT].pos = opponent_obj.pos;
		solution[OPPONENT].weight = 1e6;
	}
	//else {
	//	solution[OPPONENT].pos = add(me.pos, minus(me.pos, opponent_obj.pos));
	//	solution[OPPONENT].weight = 1e4;
	//}
	return result;
}
