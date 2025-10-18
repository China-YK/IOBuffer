#include <iostream>
#include <sys/socket.h>
#include <sys/wait.h>
void test(){
    int tcp_sock,udp_sock;
    int optval=0;
    socklen_t len=sizeof(optval);
    tcp_sock=socket(PF_INET,SOCK_STREAM,0);
    udp_sock=socket(PF_INET,SOCK_DGRAM,0);
    fprintf(stdout,"tcp_sock:%d\n",SOCK_STREAM);
    fprintf(stdout,"udp_sock:%d\n",SOCK_DGRAM);

    getsockopt(tcp_sock,//套接字描述符
        SOL_SOCKET,//通用层
        SO_TYPE,//选项名
        (void*)&optval,//存储结果的指针
                &len);//输入输出长度
    //这行代码的意思就是获取tcp_sock这个套接字通用层里面的SO_TYPE的值，并且写入optval,并且告诉可以字节数len，并返回使用字节数&len
    //至于这里为什么用(void *)，那得好好讲一下了
    //void *是通用指针类型，可以指向任意类型的数据，但它自己没有类型信息
    //int a = 10;
    //double b = 3.14;
    // char c = 'x';
    // void *p1 = &a;
    // void *p2 = &b;
    // void *p3 = &c;
    //这里为什么用void *呢，是因为有些选项的返回值不是这里的int，比如SO_LINGER返回的是struct linger,SO_PEERCRED返回的是struct ucred
    //这里的（void*）&optval的意思是：“把 optval 的地址作为一块内存块交给内核，内核会往里面写入一个整数（套接字类型）。”

    fprintf(stdout,"tcp_sock type is:%d\n",optval);
    //将获取到的值(SO_TYPE)输出出来
    optval=0;
    getsockopt(udp_sock,SOL_SOCKET,SO_TYPE,(void*)&optval,&len);
    //同样的，获取udp_sock这个套接字通用层里面的SO_TYPE的值
    fprintf(stdout,"udp_sock type is:%d\n",optval);
    optval=0;
    getsockopt(tcp_sock,SOL_SOCKET,SO_SNDBUF,(void*)&optval,&len);//获取tcp_sock套接字的发送缓冲区默认大小
    fprintf(stdout,"tcp_sock buffer_size is:%d\n",optval);//16k
    optval=1024*1024;
    std::cout<<len<<std::endl;
    setsockopt(tcp_sock,SOL_SOCKET,SO_SNDBUF,(void*)&optval,len);//手动设置tcp_sock套接字的发送缓冲区大小
    //有个坑，这里我想设置1M的缓冲区，但是实际后面输出是416k，是因为内核默认的最大允许值是208k
    //然后这里的实际生效值=min(用户值X2,maxX2);,这里显然416K<1024K,最后实际值就是416K（出于内核保护的原因）
    getsockopt(tcp_sock,SOL_SOCKET,SO_SNDBUF,(void*)&optval,&len);
    fprintf(stdout,"tcp_sock buffer_size is:%d\n",optval);
    close(tcp_sock);//wait.h头文件
    close(udp_sock);
}
void CreateServer(){
    int sock_server;
    sock_server=socket(PF_INET,SOCK_STREAM,0);
    //之前我们在学习的时候，发现有时候快速重启服务器，会出现bind failed错误，当时只知道
    //是这个端口和ip被占用了，才会bind failed。现在可以讲一下原理了
    //就是因为四次挥手里面的time-wait机制导致的(默认60s)，导致我们关闭之后重启服务器
    //会出现bind failed就是因为在time-wait(接收关闭指令仍要等待60s),所以我们在尝试
    //第二次bind这个ip和端口的时候，会显示bind failed
    //我们可以在bind代码前面加一行代码来解决这个问题setsockopt(sock_server,SQL_SOCKET,SO_RESEADDR,&opt,sizeof(opt));
    //其中opt的值为1，表示将SO_RESEADDR设置为1（默认值为0），此时就允许你在time-wait状态下重新绑定同一个端口
    //不会破坏 TCP 安全，服务器程序标准写法都加上这一句。

    // bind...
    // listen...
    // accept..
}
int main(){
    // malloc()
    test();
    CreateServer();
}