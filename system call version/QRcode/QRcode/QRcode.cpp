#include <stdio.h>
#include "HQRC.h"
#ifdef NDEBUG
#pragma comment( lib, "HQRC.lib" )
#pragma message( "Link with HQRC.lib." )
#else
#pragma comment( lib, "HQRCd.lib" )
#pragma message( "Link with HQRCd.lib." )
#endif
char* check(bool& b,bool a){b=a; return "a";}
//qrcode.exe sourcimg string outputimg version scale 
int main(int argc,char* argv[])
{	
	int version = 5;
	int level = 3; 
	int scale = 1;
	string SrcImg;
	string UserImg;
	string MaskImg;
	string Text;
	string Result="result.png";
	int US=-1;
	int BC=-1;
	bool color=false;
	if (argc==1||strcmp(argv[1],"-?")==0||strcmp(argv[1],"-h")==0)
	{
		cout<<"General Options:"<<endl;
		cout<<"-?,-h -> this help"<<endl;
		cout<<"-L [0..3] -> input QR code error correction level [L,M,Q,H] (default H)"<<endl;
		cout<<"-V [1..20] -> input QR code version (default 5)"<<endl;
		cout<<"(would change, if text length is over the capability of the version)"<<endl;
		cout<<endl;
		cout<<"HQRC Options:"<<endl;
		cout<<"-C ->set color version"<<endl;
		cout<<endl;
		cout<<"Input Options:"<<endl;
		cout<<"set the following input data name order:"<<endl;
		cout<<"-IUMT -> order: source Image, user specified strokes, mask, text"<<endl;
		cout<<"-IUT -> order: source Image, user specified strokes, text"<<endl;
		cout<<"-IMT -> order: source Image, mask, text"<<endl;
		cout<<"-IT -> order: source Image (default)"<<endl;
		cout<<endl;
		cout<<"Output Options:"<<endl;
		cout<<"-O ->input HQRC output name (default: result.png)"<<endl;
		cout<<"-S ->input output resize factor"<<endl;
		cout << endl;
		cout<<"Image Processing Options:"<<endl;
		cout<<"-US ->input unsharp mask level"<<endl;
		cout<<"-BC ->input brightness contrast level"<<endl;
		system("pause");
		return 0;
	}
	int count_unknown=0;
	for (int i = 1; i<argc; i++)
	{
		string ARG;		
		string arg = argv[i];
		for (int t = 0; t < arg.length(); ++t) { ARG += toupper(arg[t]); }

		if(strcmp(ARG.c_str(),"-IUMT")==0||strcmp(ARG.c_str(),"-IUT")==0||strcmp(ARG.c_str(),"-IMT")==0||strcmp(ARG.c_str(),"-IT")==0)
		{
			string In=ARG.c_str();
			for(int j=1;j<In.size();j++)
			{
				char t =In[j];
				switch(t)
				{
				case 'I':
					SrcImg=argv[++i];
					break;
				case 'M':
					MaskImg=argv[++i];
					break;
				case 'U':
					UserImg=argv[++i];
					break;
				case 'T':
					Text=argv[++i];
					break;
				default:
					cout<<"wrong input"<<endl;
					return 0;
				}
			}
		}
		else if (strcmp(ARG.c_str(),"-C")==0)
		{
			color=true;
		}
		else if (strcmp(ARG.c_str(),"-O")==0)
		{
			Result=argv[++i];
		}
		else if (strcmp(ARG.c_str(),"-S")==0)
		{
			scale=atoi(argv[++i]);
		}
		else if (strcmp(ARG.c_str(),"-V")==0)
		{
			version=atoi(argv[++i]);
		}
		else if (strcmp(ARG.c_str(),"-L")==0)
		{
			level=atoi(argv[++i]);
		}
		else if (strcmp(ARG.c_str(),"-US")==0)
		{
			US=atoi(argv[++i]);
		}
		else if (strcmp(ARG.c_str(),"-BC")==0)
		{
			BC=atoi(argv[++i]);
		}
		else
		{
			count_unknown++;
			bool wrong=false;
			if (count_unknown==1)
				SrcImg=(SrcImg.empty())?argv[i]:check(wrong,true);
			if (count_unknown==2)
				Text=(Text.empty())?argv[i]:check(wrong,true);
			if (count_unknown>2)
				wrong=true;
			if (wrong)
			{
				cout<<"wrong input"<<endl;
				return 0;
			}
		}
	}
	
	if(SrcImg.empty()||Text.empty())
	{
		cout<<"wrong input"<<endl;
		return 0;
	}
	
	cv::Mat srcImg=imread(SrcImg,-1);
	
	cv::Mat usermap;
	if (srcImg.empty())
	{
		cout<<"no input picture "<<SrcImg<<" "<<Text <<endl;
		return 0;
	}

	if (!UserImg.empty())
	{
		usermap=imread(UserImg,1);
	}

	cv::Mat mask;
	if (!MaskImg.empty())
	{
		mask=imread(MaskImg,0);
	}
	

	if (US>=0&&US<=2)
	{
		if (mask.empty())
		{
			HKCV::Unsharp_Mask(srcImg,srcImg,US,cv::Mat());
		}
		else
		{
			HKCV::Unsharp_Mask(srcImg,srcImg,US,mask);
		}
	}

	if (BC>=0&&BC<=2)
	{
			HKCV::Brightness_Contrast(srcImg,srcImg,BC);
	}

	cv::Mat dstimg;
	HQRCode::HQRCode(srcImg,usermap,mask,Text,dstimg,scale,color,version,level,US,BC);
	cv::Mat hqrc(dstimg.rows*1.1, dstimg.cols*1.1, dstimg.type(), cv::Scalar(255,255,255));
	dstimg.copyTo(hqrc(Rect(dstimg.rows*0.05, dstimg.cols*0.05, dstimg.rows, dstimg.cols)));
	imwrite(Result, hqrc);
	return 0;
}
