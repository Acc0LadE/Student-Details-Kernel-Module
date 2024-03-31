#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/slab.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#define NUM_OYOS 5
#define NUM_THREADS 3 // Number of threads per child process

struct oyo{
    int day;
    int month;
    int year;
    int age;
    char name[22];
    struct list_head list;
};

static LIST_HEAD(oyo_list);

static int thread_function(void *data) {
    struct oyo *person = (struct oyo *)data;
    printk(KERN_INFO "}----Child process id : %d \n", current->pid);
    printk(KERN_INFO "	   Child Process: NAME: %s, AGE: %d\n", person->name, person->age);
    printk(KERN_INFO "}--------Marks: %d\t", person->day);
    printk(KERN_INFO "         Rank: %d\t", person->month);
    printk(KERN_INFO "         Year of Passing: %d\n", person->year);
    return 0;
}

static void create_threads(struct oyo *person) {
    int i;
    for (i = 0; i < NUM_THREADS; ++i) {
        kthread_run(thread_function, person, "%s_thread%d", person->name, i);
    }
}

int simple_init(void){
    printk(KERN_INFO "Loading Module\n");
    struct oyo *persons[NUM_OYOS];
    const char *names[NUM_OYOS] = {"Pranav", "Rahul", "Ramesh", "Rudransh", "Rishi"};
    int days[NUM_OYOS] = {230, 287, 256, 235, 296};
    int months[NUM_OYOS] = {2234, 2643, 2135, 2634, 2643};
    int years[NUM_OYOS] = {2022, 2022, 2022, 2022, 2022};
    int ages[NUM_OYOS] = {19,19,19,19,19};

    int i;
    for (i = 0; i < NUM_OYOS; ++i) {
        persons[i] = kmalloc(sizeof(struct oyo), GFP_KERNEL);
        if (!persons[i]) {
            printk(KERN_ALERT "Memory Allocation failed!\n");
            return -ENOMEM;
        }
        persons[i]->day = days[i];
        persons[i]->month = months[i];
        persons[i]->year = years[i];
        persons[i]->age = ages[i];
        strncpy(persons[i]->name, names[i], sizeof(persons[i]->name));
        INIT_LIST_HEAD(&persons[i]->list);
        list_add_tail(&persons[i]->list, &oyo_list);

        // Creating threads for each OYO
        printk(KERN_INFO "parent process id : %d\n",current->pid);
        create_threads(persons[i]);
    }

    return 0;
}

void simple_exit(void){
    struct oyo *ptr, *next;
    printk(KERN_INFO "Removing Module\n");
    list_for_each_entry_safe(ptr, next, &oyo_list, list){
        list_del(&ptr->list);
        kfree(ptr);
    }
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
