#include "gurobi_c++.h"
#include <fstream> 
#include <iostream>
#include <streambuf> 
#include<stdlib.h>
#include<stdio.h>
using namespace std;
int main(int argc, char*argv)
{
//输入数据
//读取从工厂到DC的路径
ifstream ifile("C:\\Users\\hp\\Desktop\\temp.csv",ios::in);
if(ifile.is_open()==false)
{
	cout << "there is an error" << endl;
	return 0;
}


int ftodc[2][20];
for(int i =0;i<2;i++)
{
	for(int j =0;j<20;j++)
		ifile >> ftodc[i][j];  
}
/*for(int i =0;i<2;i++)
{
	for(int j =0;j<20;j++)
		cout << ftodc[i][j] << endl;  
}*/
ifile.close();

//读取从DC到DC的路径
ifstream ifile1("C:\\Users\\hp\\Desktop\\temp1.csv",ios::in);
if(ifile1.is_open()==false)
{
	cout << "there is an error" << endl;
	system("pause");
	return 0;
}


int dctodc[20][20];
for(int i =0;i<20;i++)
{
	for(int j =0;j<20;j++)
		ifile1 >> dctodc[i][j];  
}
/*for(int i =0;i<20;i++)
{
	for(int j =0;j<20;j++)
		cout << dctodc[i][j] << endl;  
}*/
ifile1.close();

//读取从DC到销售点的距离
ifstream ifile2("C:\\Users\\hp\\Desktop\\temp2.csv",ios::in);
if(ifile2.is_open()==false)
{
	cout << "there is an error" << endl;
	system("pause");
	return 0;
}
int dctos[20][475];
for(int i =0;i<20;i++)
{
	for(int j =0;j<475;j++)
		ifile2 >> dctos[i][j];  
}
/*for(int i =0;i<20;i++)
{
	for(int j =0;j<475;j++)
		cout << dctos[i][j] << endl;  
}*/
ifile2.close();

//读取销售点的需求数据
int D[475];
ifstream ifile3("C:\\Users\\hp\\Desktop\\temp3.csv",ios::in);
if(ifile3.is_open()==false)
{
	cout << "there is an error" << endl;
	system("pause");
	return 0;
}
for(int i =0;i<475;i++)
{
		ifile3 >> D[i];  
}
/*for(int i =0;i<475;i++)
{
		cout <<  D[i] << endl;  
}*/
ifile3.close();

try
{
//创建环境
GRBEnv env = GRBEnv();
GRBModel model = GRBModel(env);
//参数变量
const int C1 = 1;
const int C2 = 1;
const int C3 = 2;
const int M = INT_MAX;
//创建变量
//运输量，整数变量
GRBVar X1[2][20];
GRBVar X2[20][20];
GRBVar X3[20][475];
//0-1变量
GRBVar Y[20];
GRBVar Z1[2][20];
GRBVar Z2[20][20];

//创建决策变量
for(int i=0; i<2;i++)
{
	for(int j=0;j<20;j++)
	{
		X1[i][j] = model.addVar(0,M,0,GRB_INTEGER);//系数是否需要设置
	}
}
for(int i=0; i<20;i++)
{
	for(int j=0;j<20;j++)
	{
		X2[i][j] = model.addVar(0,M,0,GRB_INTEGER);//系数是否需要设置
	}
}
for(int i=0; i<20;i++)
{
	for(int j=0;j<475;j++)
	{
		X3[i][j] = model.addVar(0,M,0,GRB_INTEGER);//系数是否需要设置
	}
}
//设置0-1变量
//DC是否启用
for(int i =0;i<20;i++)
{
	Y[i] = model.addVar(0,1,0,GRB_BINARY);
}
for(int i=0; i<2;i++)
{
	for(int j=0;j<20;j++)
	{
		Z1[i][j] = model.addVar(0,1,0,GRB_INTEGER);//系数是否需要设置
	}
}
for(int i=0; i<20;i++)
{
	for(int j=0;j<20;j++)
	{
		Z2[i][j] = model.addVar(0,1,0,GRB_INTEGER);//系数是否需要设置
	}
}
//创建约束条件
// 1 每个DC输入输出平衡

for(int j=0;j<20;j++)
{
	GRBLinExpr input1 = 0;
	GRBLinExpr output1 = 0;
	for(int i=0;i<2;i++)
	{
		input1 +=X1[i][j];
	}
	for(int k=0;k<20;k++)
	{
		input1 +=X2[k][j];
	}
	for(int k=0;k<20;k++)
	{
		output1 += X2[j][k];
	}
	for(int l=0;l<475;l++)
	{
		output1 += X3[j][l];
	}
	model.addConstr(input1,GRB_EQUAL,output1);
}
// 2 DC年配送量约束
for(int j=0;j<20;j++)
{
	GRBLinExpr amount = 0;
	GRBLinExpr upper = 0;
	GRBLinExpr lower = 0;
	for(int l=0;l<475;l++)
	{
		amount += X3[j][l];
	}
	upper = 15000*Y[j];
	lower = 3000*Y[j];
	model.addConstr(amount,GRB_LESS_EQUAL,upper);
	model.addConstr(amount,GRB_GREATER_EQUAL,lower);
}
// 3 销售点需求量约束
for(int l=0;l<475;l++)
{
	GRBLinExpr realamount =0;
	GRBLinExpr demand =0;

	for(int j=0;j<20;j++)
	{
		realamount+=X3[j][l];
	}
	demand = D[l];
	model.addConstr(realamount,GRB_GREATER_EQUAL,demand);
}
// 4 最小起运量约束
for(int i=0;i<2;i++)
{
	for(int j=0;j<20;j++)
	{
		GRBLinExpr load =0;
		GRBLinExpr lowlimit =0;
		GRBLinExpr highlimit =0;
		load = X1[i][j];
		lowlimit = 2000*Z1[i][j]; 
		highlimit = M*Z1[i][j];
		model.addConstr(load,GRB_GREATER_EQUAL,lowlimit);
		model.addConstr(load,GRB_LESS_EQUAL,highlimit);
	}
 
}

for(int j=0;j<20;j++)
{
	for(int k=0;k<20;k++)
	{
		GRBLinExpr load =0;
		GRBLinExpr lowlimit1 =0;
		GRBLinExpr highlimit1 =0;
		load = X2[j][k];
		lowlimit1 = 2000*Z2[j][k]; 
		highlimit1 = M*Z2[j][k];
		model.addConstr(load,GRB_GREATER_EQUAL,lowlimit1);
		model.addConstr(load,GRB_LESS_EQUAL,highlimit1);
	}
 
}
//5 DC数量约束
GRBLinExpr realamount1 =0;
GRBLinExpr demand1 =10;
for(int i=0;i<20;i++)
{
	
	realamount1 += Y[i];
}
model.addConstr(realamount1,GRB_EQUAL,demand1);

//创建目标函数
GRBQuadExpr expr =0;
for(int i=0; i<2;i++)
{
	for(int j=0;j<20;j++)
	{
		expr += C1*ftodc[i][j]*X1[i][j];
	}
}
for(int i=0; i<20;i++)
{
	for(int j=0;j<20;j++)
	{
		expr += C1*dctodc[i][j]*X2[i][j];
	}
}
for(int i=0; i<20;i++)
{
	for(int j=0;j<475;j++)
	{
		expr += C2*dctos[i][j]*X3[i][j];
	}
}
for(int i=0; i<20;i++)
{
		expr += 5000000*Y[i];
}
model.setObjective(expr,GRB_MINIMIZE);

//优化
model.optimize();

//将决策变量输出进文档
/*
//运输量，整数变量
GRBVar X1[2][20];
GRBVar X2[20][20];
GRBVar X3[20][475];
//0-1变量
GRBVar Y[20];
GRBVar Z1[2][20];
GRBVar Z2[20][20];
*/
ofstream outfile1;
outfile1.open("C:\\Users\\hp\\Desktop\\X1.txt", ios::out);
for(int i=0;i<2;i++)
{
	for(int j=0;j<20;j++)
		outfile1 << X1[i][j].get(GRB_DoubleAttr_X) <<"	";
	outfile1 << "\n";
}
outfile1.close();

//
ofstream outfile2;
outfile2.open("C:\\Users\\hp\\Desktop\\X2.txt", ios::out);
for(int i=0;i<20;i++)
{
	for(int j=0;j<20;j++)
		outfile2 << X2[i][j].get(GRB_DoubleAttr_X) <<"	";
	outfile2 << "\n";
}
outfile2.close();


ofstream outfile3;
outfile3.open("C:\\Users\\hp\\Desktop\\X3.txt", ios::out);
for(int i=0;i<20;i++)
{
	for(int j=0;j<475;j++)
		outfile3 << X3[i][j].get(GRB_DoubleAttr_X) <<"	";
	outfile3 << "\n";
}
outfile3.close();



ofstream outfile4;
outfile4.open("C:\\Users\\hp\\Desktop\\Y.txt", ios::out);
for(int i=0;i<20;i++)
{
		outfile4 << Y[i].get(GRB_DoubleAttr_X) <<"	";
}
outfile4.close();


//计算各个DC的平均配送距离
for(int i=0;i<20;i++)
{
	if(Y[i].get(GRB_DoubleAttr_X)==1)
	{
		int sum =0;
		int count =0;
		for(int j=0;j<475;j++)
		{
			if(X3[i][j].get(GRB_DoubleAttr_X)>0)
			{
				count++;
				sum += dctos[i][j];
			}
		}
		cout << i << "		" ;
		cout << sum/count << endl;
	}
}

}

catch(GRBException e) 
{
cout << "Error code = " << e.getErrorCode() << endl;
cout << e.getMessage() << endl;
}
catch(...) 
{
cout << "Exception during optimization" << endl;
}
system("pause");
return 0;
}