#include "IVIView.h"

#define SOFTWARE_ID "IVIApp"
#define SOFTWARE_VERSION "IVIApp.10.03Build2021-09-15"

using std::string;
using FY_NAMESPACE::GetFYRCCPath;
using FY_NAMESPACE::GetFYFontPath;

/***************************************************************************************
 *
 * brief：  version display version information
 * input parameter discription：
 *
 * Output parameter discription：

 * return：  none
 ****************************************************************************************
 */
static void version(void)
{
	printf("%s: %s\n", SOFTWARE_ID, SOFTWARE_VERSION);
}

/***************************************************************************************
 *
 * brief:    usage display help
 * input parameter discription：
 *
 * Output parameter discription：
 * return：  none
 ****************************************************************************************
 */
static void usage()
{
	printf("%s: %s\n", SOFTWARE_ID, SOFTWARE_VERSION);
	printf("  -v    get version\n");
	printf("  -h    get help\n" );
}

/***************************************************************************************
 *
 * brief：    get_options 解析输入参数
 * 输入参数说明：
 *   getopt()
 *   1.单个字符，表示选项，
 *   2.单个字符后接一个冒号：表示该选项后必须跟一个参数。
 *   参数紧跟在选项后或者以空格隔开。该参数的指针赋给optarg。
 *   3 单个字符后跟两个冒号，表示该选项后必须跟一个参数。
 *   参数必须紧跟在选项后不能以空格隔开。该参数的指针赋给optarg
 *     extern char *optarg;  //选项的参数指针
 *     extern int optind,    //下一次调用getopt的时，从optind存储的位置处重新开始检查选项。
 *     extern int opterr,    //当opterr=0时，getopt不向stderr输出错误信息。
 *     extern int optopt;    //当命令行选项字符不包括在optstring中或者选项缺少必要的参数时，
 *                           该选项存储在optopt
 * 输出参数说明：
 *   注：如果其它地方也需要解柝argv，则需要在调用后重置optind
 * 返回值：     无
 ****************************************************************************************
 */
void get_options(int argc, char **argv)
{
	int32_t c = 0;
	int prio = -1;
	while (1)
	{
		c = getopt(argc, argv, "vh:p:");
		if (-1 == c)
		{
			break;
		}

		switch (c)
		{
		case 'v':
		{
			version();
			exit (EXIT_SUCCESS);
			break;
		}
		case 'h':
		{
			usage();
			exit (EXIT_SUCCESS);
			break;
		}
		case 'p':
		{
			if (NULL == optarg)
			{
				//output log;
				return;
			}

			prio = atoi(optarg);

			if (prio <= 0)
			{
				//output log;
			}
			continue;
		}
		default:
		{
			usage();
			exit (EXIT_SUCCESS);
			break;
		}
		}
	}
	if (prio <= 0)
	{
		prio = 12; 
	}

	int ret = pthread_setschedprio(pthread_self(), prio);
	if (-1 == ret)
	{
		//output log;
	}
	optind = 0;
}

int main(int argc, char *argv[])
{
	LOGINF("-----Main start-----");
	int ret = pthread_setname_np(pthread_self(), "IVIApp");//set thread name
	if ( ret != EOK )
	{
		LOGERR("pthread_setname_np failed!");
	}
	// if (argc <= 1)
	// {
	// 	usage();
	// 	LOGERR("argc <= 1!");
	// 	exit (EXIT_SUCCESS);oller
	// }

	get_options(argc, argv);
	LOGINF("---end get_options---,argc=%d", argc);
	for (int i = 0; i < argc; i++)
	{
		LOGDBG("--argv[%d]=%s\n",i, argv[i]);
	}
	string appParam = "show";
	string appName = "";
	if (argc > 1)
	{
		appName = argv[1];
	}

	appName = "touchcalibration";    //fyhome //touchcalibration

	LOGDBG("second appName:%s",appName.c_str());
	IVIView &View = IVIView::GetInstance();
	IVIController Controller(&View);
	Controller.SetArgv(argc,argv); 
	Controller.InitBinder(FY_HOME, false, 10000); 
	
	View.AttachController(&Controller);           // Attach the Controller

	View.MaskSignals();
	char szWindowName[256];
	sprintf(szWindowName, "1-%d-%s-%s", getpid(), "foryou", argv[1]);
    LOGDBG("szWindowName is %s",szWindowName);
	bool bRet = false;
/*   bRet = View.Init(argc, argv, "/tmp/sp/mnt/sda1/libappbase/", appName, "IVI.fyrc_device",
    				szWindowName, "/tmp/sp/mnt/sda1/libappbase/", 1920, 720, OPENGLES_RENDER_MODE); // Initialize the View
*/
	
	bRet = View.Init(argc, argv, "/application/rccs/", appName, "HMS402A.fyrc_device",
	szWindowName, "/application/rccs/", 1920, 720, CLASSIC_RENDER_MODE); // Initialize the View	 //CLASSIC_RENDER_MODE

	if (!bRet)
	{
		View.Close();
		LOGERR("main view init failed");
		return EXIT_FAILURE;
	}


	bRet = Controller.OnInit();
	if (false == bRet)
	{
		Controller.Close();
		View.Close();
		LOGERR("main Controller OnInit failed");
		return EXIT_FAILURE;
	}


	bRet = Controller.Start();
	if (false == bRet)
	{
		Controller.Close();
		View.Close();
		LOGERR("main Controller Start failed");
		return EXIT_FAILURE;
	}


	bRet = View.StartEventLoop();
	if (false == bRet)
	{
		Controller.Stop();
		Controller.Close();
		View.Close();
		LOGERR("main StartEventLoop failed");
		return EXIT_FAILURE;
	}


	View.ShowFirstWnd(appParam);


	View.WaitforSignals();

	bRet = Controller.Stop();
	if (false == bRet)
	{
		LOGERR("main Controller Stop failed");
	}

	bRet = Controller.Close();
	if (false == bRet) 
	{
		LOGERR("main Controller Close failed");
	}

    // Close the View
	bRet = View.Close();                       
	if (false == bRet)
	{
		LOGERR("main View Close failed");
		return EXIT_FAILURE;
	}

	LOGINF("-----Main end-----");
	return EXIT_SUCCESS;
}

