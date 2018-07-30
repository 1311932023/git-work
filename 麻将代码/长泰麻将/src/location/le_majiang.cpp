#include "le_majiang.h"
#include <vector>
#include <map>
#include "../common/log.h"

Matrix matrix; //手牌标识


vector<Group> groups;

int kezicount(vector<Group> & groups)
{
	int kezi = 0;	
	for (size_t i = 0; i < groups.size();i++)
	{
		if (groups[i].isKe())
		{
			kezi++;
		}
	}
	return  kezi;

} 

    
bool complete(int row, bool group)
{
	//一副花色的牌是否完整 即可分解成N个顺或刻
	if (row< 0 || row> 4)
		return false;
	Card* suits = matrix[row];

	if (suits[0] == 0)
		return true;

	//寻找第一张牌
	int j = 1;
	for (; j != NCOLS; ++j)
	{
		if (suits[j] != 0)
			break;
	}

	//刻
	if (suits[j] >= 3)
	{
		suits[j] -= 3;
		suits[0] -= 3;
		if (group)
			groups.push_back(Group(make_card(row, j), make_card(row, j), make_card(row, j)));
		bool result = complete(row, group);
		suits[j] += 3;
		suits[0] += 3;
		if (!result && group)
			groups.pop_back();
		return result;
	}

	//顺
	if (row < 3 && j < 8 && suits[j + 1] > 0 && suits[j + 2] > 0)
	{
		suits[j] -= 1;
		suits[j + 1] -= 1;
		suits[j + 2] -= 1;
		suits[0] -= 3;
		if (group)
			groups.push_back(Group(make_card(row, j), make_card(row, j + 1), make_card(row, j + 2)));
		bool result = complete(row, group);
		suits[j] += 1;
		suits[j + 1] += 1;
		suits[j + 2] += 1;
		suits[0] += 3;
		if (!result && group)
			groups.pop_back();
		return result;
	}
	return false;
}


bool can_hu(bool group ,  int start_nol  )
{
	if (group)
		groups.clear();

	int jrow = -1;  //包含将的行

	//满足3,3,3,3,2模型 
	for (int i = 0; i <=4    ; ++i)
	{
		int rem = matrix[i][0] % 3;
		if (rem == 1)
		{
			return false;
		}
		else if (rem == 2)
		{
			if (jrow != -1)
				return false;
			jrow = i;
		}
	}
	//printf("jrow %d ", jrow);

	//先计算没有将牌的花色
	for (int i = 0; i <=  4  ; ++i)
	{
		if (i != jrow)
		if (!complete(i, group))
			return false;
	}

	//该列表中包含将牌，采用逐个测试方式
	//指示除掉将后能否通过
	bool success = false;
	Card* jsuits = matrix[jrow];


	for (int j = 1; j <= 9; ++j)
	{
		if (j <=  start_nol )
			continue; 

		if (jsuits[j] >= 2)
		{
			jsuits[j] -= 2;
			jsuits[0] -= 2;
			if (group)
				groups.push_back(Group(make_card(jrow, j), make_card(jrow, j)));
			success = complete(jrow, group);
			jsuits[j] += 2;
			jsuits[0] += 2;
			if (success)
			{
				break;
			}
			else
			{
				if (group)
					groups.pop_back();
			}
		}
	}
	if (success)
		return 1;
		//hu_type = 1; // COMMON_HU;

	return success;
}


int    CanhuCount(vector<int> & all_hand_card , int end_pos  )
{
	int hu_count = 0;
	int all = all_hand_card.size();
	if (all > 14)   // || (all % 3 != 2))  // bug  . 小花麻将，丫子问题，
		return 0;
	if (end_pos > 34)
		return 0; 

	int   all_card[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
	};
	for (int st = 0; st < end_pos  ; st++)
	{
		memset(matrix, 0, sizeof(Matrix));				
		for ( int  i = 0; i <  all ; i++)
		{
			int face = get_face(all_hand_card[i]);
			int sui =  get_suits(all_hand_card[i]); 
			if (sui< 0 || sui > 4 || face< 0 || face> 9)
				return 0;			 

			matrix[sui][face]++;
			matrix[sui][0]++;  
		}
		int cur_face = get_face(all_card[st]);
		int cur_suit = get_suits(all_card[st]);
		if (cur_face< 0 || cur_face> 9 || cur_suit< 0 || cur_suit> 4)
			return 0; 

		matrix[cur_suit][cur_face]++;
		if (matrix[cur_suit][cur_face]>4)
			continue; 
		matrix[cur_suit][0]++;

		int ret = can_hu(true, 0);

		hu_count += (ret >= 1);
	}
	return hu_count;
	

}


int   hainan_check_hu( unsigned  char  *arr   ,   unsigned  char    len)
{
	if (len % 3 != 2)
		return  0;
	if (len > 14)
		return 0;

	memset(matrix, 0, sizeof(Matrix));
	for (  unsigned  char  i = 0; i < len; i++)
	{
		int face = get_face(arr[i]);
		int sui = get_suits(arr[i]);
		if (sui< 0 || sui> 4 || face< 0 || face> 9)
			return 0;


		matrix[sui][face]++;
		matrix[sui][0]++;
	}
	int ret = can_hu(true, 0);
	return  ret;
}


int le_check_hu(  int    *arr,  int   len)
{
	if (len % 3 != 2)
		return  0;  
	if (len > 14 )
		return 0;

	memset(matrix, 0, sizeof(Matrix));
	for (int i = 0; i < len;  i++)
	{
		int face = get_face( arr[i]);
		int sui = get_suits(arr[i]);
		if (sui< 0 || sui> 4 || face< 0 || face> 9)
			return 0;


		matrix[sui][face]++;
		matrix[sui][0]++;
	}
	int ret = can_hu(true, 0 );
	return  ret; 
} 

int  GetMaxKeCount(vector<int>  & all_han_card  )
{	
	int ke_max = 0;	
	for (int st = 0; st < 10; st++)
	{
		memset(matrix, 0, sizeof(Matrix));
		int all = all_han_card.size();
		for ( int  i = 0; i <  all ; i++)
		{
			int face = get_face(all_han_card[i]);
			int sui =  get_suits(all_han_card[i]);
			if (sui< 0 || sui> 4 || face< 0 || face> 9)
				return 0;			
			matrix[sui][face]++;
			matrix[sui][0]++;
		}
		int ret = can_hu(true, st);
		
		if (ret)
		{
			int ke = kezicount(groups);
			if (ke > ke_max)
			{
				ke_max = ke;
			}
			if (ke >= 4)
				return 4;			
		}

	}

	return  ke_max;
}

int  hu_has_ke(vector<int>  & all_han_card)
{	
	//for (int st = 0; st < 10; st++)
	{
		memset(matrix, 0, sizeof(Matrix));
		int all = all_han_card.size(); 
		for (int i = 0; i < all; i++)
		{
			int face = get_face(all_han_card[i]);
			int sui = get_suits(all_han_card[i]);
			if (sui< 0 || sui> 4 || face< 0 || face> 9)
				return 0;
			matrix[sui][face]++;
			matrix[sui][0]++;
		}
		int ret = can_hu(true, 0 ); 
		if (ret)
		{
			return kezicount(groups); 			
		}
	}
	return  0;
}


int  get_all_ting_card_vec(vector<int> &  Ting_Vec, vector<int> & all_card_vec)
{
	int all = all_card_vec.size(); 
	if ((all + 1) % 3 != 2)
		return 0;
	if (all > 14)
		return 0; 

	int  all_card[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31,0x32,0x33,0x34,0x41,0x42,0x43 ,
	};
	for (int st = 0; st < 34  ; st++)
	{
		memset(matrix, 0, sizeof(Matrix));
		for ( int  i = 0; i <   all ; i++)
		{
			int face = get_face(all_card_vec[i]);
			int sui =  get_suits(all_card_vec[i]); 
			if (sui< 0 || sui> 4 || face< 0 || face> 9)
				return  0;
			

			matrix[sui][face]++; 
			matrix[sui][0]++;
		}

		int  suit = get_suits(all_card[st]);
		int  face = get_face(all_card[st]);
		if (suit< 0 || suit> 4 || face< 0 || face> 9)
			return  0;
		matrix[suit][face]++;
		if (matrix[suit][face] > 4)
			continue;
		matrix[suit][0]++;

		int ret = can_hu(1, 0);
		if (ret > 0)
			Ting_Vec.push_back(all_card[st]);
	}
	return  0;

}

int  IsAllShunzi()
{
	vector<Group>::iterator  it = groups.begin();
	for (; it != groups.end(); ++it)
	{
		if (it->isJiang())
			continue; 
		if (it->isKe())
			return  0;
	}
	return  1;
}

int  IsAllKezi()
{
	vector<Group>::iterator  it = groups.begin();
	for (; it != groups.end(); ++it)
	{
		if (it->isJiang())
			continue;
		if (it->isShun())
			return  0;
	}
	return  1;
}


int  jiang_after_hu()
{
	vector<Group>::iterator  it = groups.begin();
	for (; it != groups.end(); ++it)
	{
		if (it->isJiang())
			return it->first();
	}
	return  -1;  
}

int hu_if_add( vector<int> & all_card_vec , int end_pos    )
{
	int all = all_card_vec.size();
	if ((all + 1) % 3 != 2)
		return 0;
	if (all == 0 || end_pos > 34 )
		return 0;  

	int  all_card[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43, 
	};
	for (int st = 0; st <   end_pos   ; st++)
	{	
		memset(matrix, 0, sizeof(Matrix));
		for (size_t i = 0; i < all_card_vec.size(); i++)
		{ 
			int face =  get_face(all_card_vec[i]);
			int sui  =  get_suits(all_card_vec[i]);
			if (sui< 0 || sui> 4 || face< 0 || face> 9)
				return 0;

			matrix[sui][face]++;
			matrix[sui][0]++;
		}

		int  suit = get_suits(all_card[st]);
		int  face = get_face(all_card[st]);
		if (suit< 0 || suit> 4 || face< 0 || face>  9)
			return 0;
		matrix[suit][face]++;
		if (matrix[suit][face] > 4)
			continue; 		
		matrix[suit][0]++;

		int ret = can_hu(1, 0);
		if (ret > 0)
			return 1;
	}
	
	return  0;
	
}



#if 1
 
#define  MJ_LAZI   0x70 

bool laizi_complete(int row, bool group , int &n_laizi  )
{
	//一副花色的牌是否完整 即可分解成N个顺或刻
	if (row< 0 || row> 4)
		return false;
	Card* suits = matrix[row];
	if (suits[0] == 0)
		return true;

	//寻找第一张牌
	int j = 1;
	for (; j != NCOLS; ++j)
	{
		if (suits[j] != 0)
			break;
	}

	//刻
	if (suits[j] >= 3)
	{
		suits[j] -= 3;
		suits[0] -= 3; 
		if (group)
			groups.push_back( Group(make_card(row, j), make_card(row, j), make_card(row, j)) );

		bool result = laizi_complete(row, group ,  n_laizi );
		if (!result)
		{
			suits[j] += 3;
			suits[0] += 3;
			if (!result && group)
				groups.pop_back();
			//return result;
		}
		else
			return result;
	}
	if (suits[j] >= 2 && n_laizi>= 1)
	{
		suits[j] -= 2;
		suits[0] -= 2; 
		n_laizi--;
		if (group)
			groups.push_back( Group(make_card(row, j), make_card(row, j), MJ_LAZI));
		bool result = laizi_complete(row, group, n_laizi);
		if (!result)
		{
			suits[j] += 2;
			suits[0] += 2;
			n_laizi++;
			if (!result && group)
				groups.pop_back();
			//return result;
		}	
		else
			return result;
		
	}

	// int arr[] = {  0x1,0x2 ,0x11 ,0x11, 0x4,0x4 



	//顺
	if (row < 3 && j < 8 && suits[j + 1] > 0 && suits[j + 2] > 0)
	{
		suits[j] -= 1;
		suits[j + 1] -= 1;
		suits[j + 2] -= 1;
		suits[0] -= 3;
		if (group)
			groups.push_back( Group(make_card(row, j), make_card(row, j + 1), make_card(row, j + 2)));
		
		bool result = laizi_complete(row, group , n_laizi );
		if (!result)
		{
			suits[j] += 1;
			suits[j + 1] += 1;
			suits[j + 2] += 1;
			suits[0] += 3;
			if (!result && group)
				groups.pop_back();
			return result;
		}
		else
			return result;
	}

	//顺
	if (row < 3 && j <= 8 && suits[j + 1] > 0 &&  n_laizi>= 1 )
	{
		suits[j] -= 1;
		suits[j + 1] -= 1;
		//suits[j + 2] -= 1;
		suits[0] -= 2;
		n_laizi--;
		if (group)
			groups.push_back( Group( make_card(row, j), make_card(row, j + 1),MJ_LAZI ) );

		bool result = laizi_complete(row, group , n_laizi);
		if (!result)
		{
			suits[j] += 1;
			suits[j + 1] += 1;
			//suits[j + 2] += 1;
			suits[0] += 2;
			n_laizi++;
			if (!result && group)
				groups.pop_back();
			return result;
		}
		else
			return result;
	}


	//顺
	if (row < 3 && j < 8 && suits[j + 2] > 0 && n_laizi >= 1)
	{
		suits[j] -= 1;
		suits[j + 2] -= 1;
		//suits[j + 2] -= 1;
		suits[0] -= 2;
		n_laizi--;
		if (group)
			groups.push_back( Group(make_card(row, j), make_card(row, j + 2 ), MJ_LAZI ) ) ;

		bool result = laizi_complete(row, group, n_laizi);
		if (!result)
		{
			suits[j] += 1;
			suits[j + 2] += 1;
			//suits[j + 2] += 1;
			suits[0] += 2;
			n_laizi++;
			if (!result && group)
				groups.pop_back();
			return result;

		}		
		else
			return result;
	}
	if (suits[j] >= 1 && n_laizi >= 2 )
	{
		suits[j] -= 1;
		suits[0] -= 1;
		n_laizi -= 2;
		if (group)
			groups.push_back( Group( make_card(row, j), MJ_LAZI , MJ_LAZI) );

		bool result = laizi_complete(row, group, n_laizi);
		if (!result)
		{
			suits[j] += 1;
			suits[0] += 1;
			n_laizi += 2;
			if (!result && group)
				groups.pop_back();
			return result;
		}
		return result;
		
	}

	//printf("j == %d \n",  j );


	return false;
}



int  laizi_le_can_hu_jiang258(  unsigned char  *arr, unsigned  char  len, int n_lazi )
{
	if ( ((len + n_lazi) % 3 != 2) || (len+ n_lazi> 14 )) 
		return 0;
	if (len == 0)
		return  1;

	if (n_lazi == 0)
	{
		memset(matrix, 0, sizeof(Matrix));
		for (int i = 0; i < len; i++)
		{
			int face = get_face(arr[i]);
			int sui = get_suits(arr[i]);
			if (sui< 0 || sui> 4 || face< 0 || face> 9)
				return 0;
			matrix[sui][face]++;
			matrix[sui][0]++;
		}
		if (can_hu(1, 0))
		{
			vector<Group>::iterator it = groups.begin();
			for (; it != groups.end(); ++it)
			{
				if (it->isJiang())
				{
					int ct = it->first(); 
					int face_l = get_face(ct); 
					printf("ct %x  \n ", ct);
					if (face_l == 2 || face_l == 5 || face_l == 8)
						return 1;
				}
			}
			return  0;
		}
	}
	else
	{
		int  first_laizi = n_lazi; 
		if (n_lazi >= 2)
		{
			memset(matrix, 0, sizeof(Matrix));
			for (int i = 0; i < len; i++)
			{
				int face = get_face(arr[i]);
				int sui = get_suits(arr[i]);
				if (sui< 0 || sui> 4 || face< 0 || face> 9)
					return 0;
				matrix[sui][face]++;
				matrix[sui][0]++;
			}

			groups.clear();
			bool canhu = true;
			int cur_laizi = n_lazi - 2; 
			for (int row = 0; row < 5; row++)
			{
				int ret = laizi_complete(row, true, cur_laizi  );
				if (ret <= 0)
				{
					canhu = 0;
					break;
				}
			}
			if (canhu)
			{
				//debug 
				for (vector<Group>::iterator it=groups.begin(); it!= groups.end(); ++it)
				{
					if (it->isJiang())
						printf("jiang %x \n", it->first());
					else if(it->isShun())
						printf("shun %x  \n", it->first()); 
					else
						printf("kezi %x  \n", it->first());
				}
				//end dubug 
				return canhu;
			}
			
		}
		map<int, int> all_map;
		for (int i = 0; i < len; i++)
			all_map[arr[i]]++;
		

		for (map<int, int>::iterator it = all_map.begin(); it != all_map.end(); ++it)
		{
			
			if (!(get_face(it->first) == 2 || get_face(it->first)==5  || get_face(it->first) == 8) ) 
				continue; 
			printf("jiang %x \n ", it->first); 
			memset(matrix, 0, sizeof(Matrix));
			for (int i = 0; i < len; i++)
			{
				int face = get_face(arr[i]);
				int sui = get_suits(arr[i]);
				if (sui< 0 || sui> 4 || face< 0 || face> 9)
					return 0;
				matrix[sui][face]++;
				matrix[sui][0]++;
			}
			

			n_lazi = first_laizi;
			int  card = it->first;
			int  ct = it->second;
			if (ct >= 2)
			{
				int suit = get_suits(card);
				int face = get_face(card);
				matrix[suit][face] -= 2;
				matrix[suit][0] -= 2;
				int row = 0;
				bool canhu = true;
				printf("card [%x] laize %d \n", card, n_lazi);
				groups.clear();
				for (row = 0; row < 5; row++)
				{
					int ret = laizi_complete(row, true, n_lazi);
					if (ret <= 0)
					{
						canhu = 0;
						break;
					}
				}
				//printf("row --%d\n", row);
				if (canhu)
					return  1;
				matrix[suit][face] += 2;
				matrix[suit][0] += 2;


			}
			else if (ct == 1 && n_lazi >= 1)
			{
				printf(" card [%x] laize %d \n", card, n_lazi);
				int suit = get_suits(card);
				int face = get_face(card);
				matrix[suit][face] -= 1;
				matrix[suit][0] -= 1;
				n_lazi--;
				int row = 0;
				bool canhu = 1;
				groups.clear();
				for (row = 0; row < 5; row++)
				{
					int ret = laizi_complete(row, true, n_lazi);
					if (ret <= 0)
					{
						canhu = 0;
						break;
					}
				}
				//printf("row --%d\n", row);

				if (canhu)
					return  1;
				matrix[suit][face] += 1;
				matrix[suit][0] += 1;
				n_lazi++;
			}


		}
	}
	return  0;
}


int  laizi_le_can_hu(int  *arr, int len , int  n_lazi  )
{
	if ((len + n_lazi) % 3 != 2)
		return 0;
	
	if (n_lazi== 0 )
	{
		memset(matrix, 0, sizeof(Matrix));
		for (int i = 0; i < len; i++)
		{
			int face = get_face(arr[i]);
			int sui = get_suits(arr[i]);
			if (sui< 0 || sui> 4 || face< 0 || face> 9)
				return 0;
			matrix[sui][face]++;
			matrix[sui][0]++;
		}
		return can_hu(1, 0);
	}
	else
	{
		map<int, int> all_map;
		for (int i = 0; i < len; i++)
			all_map[arr[i]]++;
		int  first_laizi = n_lazi;

		for (map<int, int>::iterator it = all_map.begin(); it != all_map.end(); ++it)
		{
			printf("ss %x  laize %d   \n", it->first , n_lazi ); 

//#if 1
			memset(matrix, 0, sizeof(Matrix));
			for (int i = 0; i < len; i++)
			{
				int face = get_face(arr[i]);
				int sui = get_suits(arr[i]);
				if (sui< 0 || sui> 4 || face< 0 || face> 9)
					return 0;
				matrix[sui][face]++;
				matrix[sui][0]++;
			}
//#endif
	
			n_lazi = first_laizi;  
			int  card  = it->first; 
			int  ct = it->second;
			if (ct >= 2)
			{
				int suit = get_suits(card);
				int face = get_face(card);
				matrix[suit][face] -= 2;
				matrix[suit][0] -= 2;
				int row = 0;
				bool canhu = true ;
				printf("card [%x] laize %d \n",  card ,  n_lazi);
				groups.clear();
				for ( row = 0; row < 5 ; row++)
				{
					int ret = laizi_complete(row, true, n_lazi);
					if (ret <= 0)
					{
						canhu =  0;
						break;
					}
				} 
				//printf("row --%d\n", row);
				if ( canhu  )
					return  1;
				matrix[suit][face] += 2;
				matrix[suit][0] += 2;
			
			
			}
			else if (ct == 1 && n_lazi >= 1)
			{
				printf(" card [%x] laize %d \n", card, n_lazi); 
				int suit = get_suits(card);
				int face = get_face(card);
				matrix[suit][face] -= 1;
				matrix[suit][0] -= 1;
				n_lazi--;
				int row = 0;
				bool canhu = 1 ;
				groups.clear();
				for (row = 0; row < 5 ; row++)
				{
					int ret = laizi_complete(row, true, n_lazi);
					if (ret <= 0)
					{ 
						canhu= 0 ;
						break;
					}
				}
				//printf("row --%d\n", row);

				if ( canhu  )
					return  1;
				matrix[suit][face] += 1;
				matrix[suit][0] += 1;
				n_lazi++; 
			}


		}
	}
	return  0;  
	
}


int   laizi_chibi_check_hu(  unsigned  char   *arr,  unsigned  char   len, int  n_lazi)
{
	if ((len + n_lazi) % 3 != 2)
		return 0;
	if (len == 0)
		return 1; 
	bool bCanHu = false;
	int bmaxkenum = 0;
	if (n_lazi == 0)
	{
		memset(matrix, 0, sizeof(Matrix));
		for (int i = 0; i < len; i++)
		{
			int face = get_face(arr[i]);
			int sui = get_suits(arr[i]);
			if (sui< 0 || sui> 4 || face< 0 || face> 9)
				return 0;
			matrix[sui][face]++;
			matrix[sui][0]++;
		}
		return can_hu(1, 0);
	}
	else
	{
		map<int, int> all_map;
		for (int i = 0; i < len; i++)
			all_map[arr[i]]++;
		int  first_laizi = n_lazi;

		for (map<int, int>::iterator it = all_map.begin(); it != all_map.end(); ++it)
		{
			printf("ss %x  laize %d   \n", it->first, n_lazi);

			//#if 1
			memset(matrix, 0, sizeof(Matrix));
			for (int i = 0; i < len; i++)
			{
				int face = get_face(arr[i]);
				int sui = get_suits(arr[i]);
				if (sui< 0 || sui> 4 || face< 0 || face> 9)
					return 0;
				matrix[sui][face]++;
				matrix[sui][0]++;
			}
			//#endif

			n_lazi = first_laizi;
			int  card = it->first;
			int  ct = it->second;
			if (ct >= 2)
			{
				int suit = get_suits(card);
				int face = get_face(card);
				matrix[suit][face] -= 2;
				matrix[suit][0] -= 2;
				int row = 0;
				bool canhu = true;
				printf("card [%x] laize %d \n", card, n_lazi);
				groups.clear();
				for (row = 0; row < 5; row++)
				{
					int ret = laizi_complete(row, true, n_lazi);
					if (ret <= 0)
					{
						canhu = 0;
						break;
					}
				}
				//printf("row --%d\n", row);
				if (canhu)
				{
					for (size_t i = 0; i < groups.size(); i++)
					{
					//	log_info(" groups[i = %d].first() = %x", i, groups[i].first());
					//	log_info(" groups[i = %d].second() = %x", i, groups[i].second());
					//	log_info(" groups[i = %d].third() = %x", i, groups[i].third());

					}
					bCanHu = true;
					if (bmaxkenum < kezicount(groups))
					{
					//	log_info("bmaxkenum = %x", bmaxkenum);
						bmaxkenum = kezicount(groups);
					//	log_info("bmaxkenum = %x", bmaxkenum);
					}					
				}
				matrix[suit][face] += 2;
				matrix[suit][0] += 2;


			}
			memset(matrix, 0, sizeof(Matrix));
			for (int i = 0; i < len; i++)
			{
				int face = get_face(arr[i]);
				int sui = get_suits(arr[i]);
				if (sui < 0 || sui> 4 || face < 0 || face> 9)
					return 0;
				matrix[sui][face]++;
				matrix[sui][0]++;
			}
			//#endif

			n_lazi = first_laizi;
			if (ct == 1 && n_lazi >= 1)
			{
				printf(" card [%x] laize %d \n", card, n_lazi);
				int suit = get_suits(card);
				int face = get_face(card);
				matrix[suit][face] -= 1;
				matrix[suit][0] -= 1;
				n_lazi--;
				int row = 0;
				bool canhu = 1;
				groups.clear();
				for (row = 0; row < 5; row++)
				{
					int ret = laizi_complete(row, true, n_lazi);
					if (ret <= 0)
					{
						canhu = 0;
						break;
					}
				}
				//printf("row --%d\n", row);

				if (canhu)
				{
					bCanHu = true;
					for (size_t i = 0; i < groups.size(); i++)
					{
					//	log_info(" groups[i = %d].first() = %x", i, groups[i].first());
					//	log_info(" groups[i = %d].second() = %x", i, groups[i].second());
					//	log_info(" groups[i = %d].third() = %x", i, groups[i].third());

					}
					if (bmaxkenum < kezicount(groups))
					{
					//	printf("bmaxkenum --%d\n", bmaxkenum);
					//	log_info("bmaxkenum = %x", bmaxkenum);

						bmaxkenum = kezicount(groups);
					}
				}
				matrix[suit][face] += 1;
				matrix[suit][0] += 1;
				n_lazi++;
			}
			memset(matrix, 0, sizeof(Matrix));
			for (int i = 0; i < len; i++)
			{
				int face = get_face(arr[i]);
				int sui = get_suits(arr[i]);
				if (sui < 0 || sui> 4 || face < 0 || face> 9)
					return 0;
				matrix[sui][face]++;
				matrix[sui][0]++;
			}
			//#endif

			n_lazi = first_laizi;
			if (ct >= 2)
			{
				int suit = get_suits(card);
				int face = get_face(card);
				matrix[suit][face] -= 1;
				matrix[suit][0] -= 1;
				n_lazi--;
				int row = 0;
				bool canhu = true;
				printf("card [%x] laize %d \n", card, n_lazi);
				groups.clear();
				for (row = 0; row < 5; row++)
				{
					int ret = laizi_complete(row, true, n_lazi);
					if (ret <= 0)
					{
						canhu = 0;
						break;
					}
				}
				//printf("row --%d\n", row);
				if (canhu)
				{
					for (size_t i = 0; i < groups.size(); i++)
					{
					//	log_info(" groups[i = %d].first() = %x", i, groups[i].first());
					//	log_info(" groups[i = %d].second() = %x", i, groups[i].second());
					//	log_info(" groups[i = %d].third() = %x", i, groups[i].third());

					}
					bCanHu = true;
					if (bmaxkenum < kezicount(groups))
					{
					//	log_info("bmaxkenum = %x", bmaxkenum);
						bmaxkenum = kezicount(groups);
					//	log_info("bmaxkenum = %x", bmaxkenum);
					}
				}
				matrix[suit][face] += 2;
				matrix[suit][0] += 2;
			}
		}
	}
	if (bCanHu==true)
	{
		return bmaxkenum;
	}
	return  0;

}



#endif


//#if    0  

//int len= 0 ;
//unsigned  char all_card[] =
//{
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
//	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
//	0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,  //34 
//};
//#endif
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <sys/time.h>
//#include <algorithm>


//
//int main()
//{
//	//int arr[] = { 0x1, 0x1 };
//	//int arr[] = { 0x1, 0x1 , 0x1,0x2, 0x3,   0x41, 0x41, 0x41  };
//	//int arr[] = { 0x1 };
//	////int arr[] = { 0x1, 0x2, 0x4, 0x4 };
//	//int arr[ 6] = {  0x16,  0x17, 0x18,  0x22,  0x24,  0x28 };
//	// int arr[ 11 ] = {  0x9 , 0x24,0x24,0x25,0x25 ,0x26,0x26 , 0x27,0x28,0x28,0x29 };
//	//int  arr[] = { 0x3,0x4,0x5 ,0x8,0x8,0x8 };
//
//	//int  arr[] = { 0x3, 0x3,0x3,0x4,05, 0x17,0x21 };
//	unsigned char  arr[] = {    0x9,0x9  , 0x2,0x2  };
//	
//	
//	int  len = sizeof(arr) / sizeof(arr[0]); 
//	int   kezi = 2; 
//	
//	int  ret = laizi_chibi_check_hu(arr , len   ,  1 );
//	   
//
//	printf("ret  len %d  %d  \n ", len  ,  ret  ); 
//	 // return  0;
//	vector<Group>::iterator  it = groups.begin();
//	for (; it != groups.end(); ++it)
//	{
//		if (it->isJiang() )
//		{
//			printf("elem %x %x \n", it->first() , it->second() ); 
//		}
//		else
//		{
//			printf("elem %x %x %x  \n", it->first(), it->second() , it->third() );
//		}
//	}
//	//return 0;  
//	
//	
//	//struct timeval _tv; 
//	//gettimeofday(&_tv, NULL); 
//	//struct tm _tm; 
//	//localtime_r(&_tv.tv_sec, &_tm); 
//	//printf("dbug|%d-%02d-%02d %02d:%02d:%02d.%06ld \n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec, _tv.tv_usec);
//	//
//
//	//for (int tt = 0; tt < 10000; tt++)
//	//{
//	//	len = 0;		
//	//	memset(arr, 0, sizeof(arr)); 
//	//	for (int i = 0; i < 9; i++)
//	//	{
//	//		int ct = rand() % 34;
//	//		arr[len++] = all_card[ct]; 
//	//	}			
//	//	ret = laizi_le_can_hu(arr, len,    kezi );
//	//	if (ret)
//	//	{
//	//		sort(arr  , arr+len); 
//	//		for (int i = 0; i < 9; i++)
//	//			printf(" [%x] ", arr[i]);
//	//		printf("\n ret --len[%d]----------%d \n", 4, ret); 
//	//	}	
//	//}
//	//for (int tt = 0; tt < 10000; tt++)
//	//{
//	//	len = 0;
//	//	memset(arr, 0, sizeof(arr));
//	//	for (int i = 0; i < 13 ; i++)
//	//	{
//	//		int ct = rand() % 34;
//	//		arr[len++] = all_card[ct];
//	//	}
//	//	ret = laizi_le_can_hu(arr, len,  1);
//	//	if (ret)
//	//	{
//	//		sort(arr, arr + len);
//	//		for (int i = 0; i < 13 ; i++)
//	//			printf(" [%x] ", arr[i]);
//	//		printf("\n ret --len[%d]----------%d \n", 4, ret);
//	//	}
//	//} 	
//	//gettimeofday(&_tv, NULL);
//	//localtime_r(&_tv.tv_sec, &_tm);
//	//printf("dbug|%d-%02d-%02d %02d:%02d:%02d.%06ld  \n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec, _tv.tv_usec);
//
//	//
//
//	//return 0 ;
//
//	//int arr[14] = {0} ;
//	
//	//for(int i = 0 ; i< 100; i++)
//	//{
// //        int c =  rand()  % 34 ;
// //        arr[0] =  all_card[c] ;
// //        for(int j = 1 ; j<= 3 ;j++)
// //        {
// //        	 c =  rand()  % 34 ;         
// //        	arr[j]  = all_card[c] ;
// //        }
// //        len=1 ;
// //        int ret = laizi_le_can_hu(arr, 4 ,  kezi  ); 
// //        for(int j = 0 ; j<= 3 ; j++)
// //        	printf("%x  ",  arr[j]);
//
//	//	 printf("ret   card[%x]--len[%d]----------%d\n", arr[0] ,  len ,  ret);
//
//
//	//}
//
//
//	return 0;
//
//
//}
//#endif