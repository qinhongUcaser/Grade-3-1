#include "time.h"
#include "test3.h"
#include "lock.h"
#include "sched.h"
#include "stdio.h"
#include "mailbox.h"
#include "syscall.h"


struct task_info sq_task = {"SunQuan", (uint32_t)&SunQuan, USER_PROCESS};
struct task_info lb_task = {"LiuBei", (uint32_t)&LiuBei, USER_PROCESS};
struct task_info cc_task = {"CaoCao", (uint32_t)&CaoCao, USER_PROCESS};

void SunQuan(void)
{
    int i;
    // mbox_init();
    // debuginfo(1234);
    mailbox_t *pub = mbox_open("SunQuan-Publish-PID");
    pid_t myPid = sys_getpid();

    /* Send PID twice, once for LiuBei,
     * and once for the CaoCao */

    sys_move_cursor(0, 0);
    printf("SunQuan(%d): Hello, I am SunQuan          ", myPid);


    mbox_send(pub, &myPid, sizeof(pid_t));
    mbox_send(pub, &myPid, sizeof(pid_t));

    // printf("\n%d",pub->data[1]);
    /* Find LiuBei's PID */
    mailbox_t *sub = mbox_open("LiuBei-Publish-PID");
    // printf("\n\n%d",sub->i);

    for (i = 0;;i++)
    {
        pid_t liubei;

        sys_move_cursor(0, 0);
        printf("[SunQuan](%d): Where are you Liubei?   (%d)             ", myPid,i);
        mbox_recv(sub, &liubei, sizeof(pid_t));

        sys_move_cursor(0, 0);
        printf("[SunQuan](%d): I'm waiting for Liubei (%d)          ", myPid, liubei);
        sys_waitpid(liubei);

        sys_move_cursor(0, 0);
        printf("[SunQuan](%d): I'm coming to save you, LiuBei!", myPid);

        sys_sleep(1);
        sys_spawn(&lb_task);
        mbox_send(pub, &myPid, sizeof(pid_t));
    }
}

void LiuBei(void)
{
    int i;
    mailbox_t *pub = mbox_open("LiuBei-Publish-PID");
    pid_t myPid = sys_getpid();

    /* Send PID twice, once for sunquan Hood,
     * and once for the CaoCao */
    mbox_send(pub, &myPid, sizeof(pid_t));
    mbox_send(pub, &myPid, sizeof(pid_t));

    // printf("\n\n\n%d",pub->i);
    /* Find sunquan's PID */
    mailbox_t *sub = mbox_open("SunQuan-Publish-PID");

    // sys_move_cursor(0, 1);
    // printf("LiuBei(%d): Hello, I am Liubei          ", myPid);

    for (i = 0;;i++)
    {
        pid_t aramis;
        
        sys_move_cursor(0, 1);
        printf("[LiuBei](%d): Where are you SunQuan? (%d)         ", myPid,i);
        mbox_recv(sub, &aramis, sizeof(pid_t));

        sys_move_cursor(0, 1);
        printf("[LiuBei](%d): I'm waiting for SunQuan (%d)    ", myPid, aramis);
        sys_waitpid(aramis);

        sys_move_cursor(0, 1);
        printf("[LiuBei](%d): I'm coming to save you, SunQuan!", myPid);

        sys_sleep(1);
        // debuginfo(pcb[2].tag);
        sys_spawn(&sq_task);
        // printf("\n\n\n\n\n%d %d",current_running->pid,queue_count(&ready_queue));
        mbox_send(pub, &myPid, sizeof(pid_t));
        // debuginfo(pub->count);
    }
}

void CaoCao(void)
{
    uint32_t myRand;
    pid_t myPid = sys_getpid();

    mailbox_t *subSunQuan = mbox_open("SunQuan-Publish-PID");
    mailbox_t *subLiuBei = mbox_open("LiuBei-Publish-PID");

    int i;
    pid_t sunquan, liubei;
    mbox_recv(subSunQuan, &sunquan, sizeof(pid_t));
    mbox_recv(subLiuBei, &liubei, sizeof(pid_t));
    
    // printf("\n\n%d %d",sunquan,liubei);
    for (i = 0;;i++)
    {
        sys_move_cursor(0, 2);
        printf("[CaoCao](%d): I am working... muahaha (%d)", myPid,i);

        sys_sleep(5);

        sys_move_cursor(0, 3);
        printf("[CaoCao](%d): I have my decision! ", myPid);

        switch (i % 2)
        {
        case 0:
            sys_move_cursor(0, 4);
            printf("[CaoCao](%d): I will kill SunQuan (%d)!  ", myPid, sunquan);
            sys_sleep(1);
            
            sys_move_cursor(0, 5);
            printf("[CaoCao]biu biu biu ~~~~~~ AAAAAAAA SunQuan is dead QAQ.");
            sys_kill(sunquan);
            // printf("%d",subSunQuan->count);// debuginfo(1234);
            mbox_recv(subSunQuan, &sunquan, sizeof(pid_t));
            // debuginfo(1234);
            sys_move_cursor(0, 6);
            printf("[CaoCao](%d): Oops! SunQuan(%d) lives!                 ", myPid, sunquan);
            // debuginfo(queue_count(&ready_queue));
            break;
        case 1:
            sys_move_cursor(0, 4);
            printf("[CaoCao](%d): I will kill LiuBei(%d)! ", myPid, liubei);
            sys_sleep(1);
            
            sys_move_cursor(0, 5);
            printf("[CaoCao]biu biu biu ~~~~~~ AAAAAAAA Liubei is dead QAQ.");
            sys_kill(liubei);
            
            sys_move_cursor(0, 6);
            mbox_recv(subLiuBei, &liubei, sizeof(pid_t));
            printf("[CaoCao](%d): Oops! LiuBei(%d) is alive again! ", myPid, liubei);
            break;
        }

        sys_sleep(2);
    }
}
