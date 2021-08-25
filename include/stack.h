
#define Stack(type)\
struct Stack##type{\
    struct StackNode##type{\
        type item;\
        struct StackNode##type *nextNode;\
    } *head;\
}

#define stack_count(type, stack, num_var){\
    if(stack.head == NULL){\
        num_var = 0;\
    }\
}\

#define stack_push(type, stack, value){\
    typedef struct StackNode##type{\
        type item;\
        struct StackNode##type *nextNode;\
    }StackNode##type;\
\
    typedef struct Stack##type{\
        StackNode##type *head;\
    }Stack##type;\
\
\
    StackNode##type *newNode = (StackNode##type*) malloc(sizeof(StackNode##type*));\
    memcpy(&newNode->item, &value, sizeof(type));\
\
\
    if(stack.head == NULL){\
        stack.head = newNode;\
    }else{\
        newNode->nextNode = stack.head;\
        stack.head = newNode;\
    }\
}   