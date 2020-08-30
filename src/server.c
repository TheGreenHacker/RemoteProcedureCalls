#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "rpc.h"
#include "person_t/person_t.h"
#include "../sentinel.h"
#include "../serialize.h"
#include "ll_t/ll_t.h"
#include "serialize_int/serialize_int.h"
#include "ll_pair_t/ll_pair_t.h"

int flag = 1; // indicates if server is running

void signal_handler(int signal_num) {
    if (signal_num == SIGINT) {
        flag = 0;
    }
}

/* Returns list of person_t objects whose age exceeds 60 */
ll_t *get_senior_citizens(const ll_t *l) {
    ll_t *senior_citizens = init_list();
    ll_node_t *curr = l->head;
    
    while (curr) {
        person_t *p = (person_t *)curr->data;
        if (p->age >= 60) {
            person_t *p_copy = malloc(sizeof(person_t));
            memcpy(p_copy, p, sizeof(person_t));
            add_data(senior_citizens, p_copy);
        }
        curr = curr->next;
    }
    
    return senior_citizens;
}

ll_t *get_senior_citizens_server_stub_unmarshal(ser_buff_t *recv_b) {
    return get_senior_citizens(deserialize_ll_t(recv_b, deserialize_person_t_wrapper));
}

void get_senior_citizens_server_stub_marshal(ll_t *l, ser_buff_t *send_b) {
    serialize_ll_t(l, send_b, serialize_person_t_wrapper);
}

/* Returns record of person_t object for eldest person in list */
person_t *get_eldest_citizen(const ll_t *l) {
    person_t *eldest_p = NULL;
    ll_node_t *curr = l->head;
    
    while (curr) {
        person_t *p = (person_t *)curr->data;
        if (!eldest_p || p->age > eldest_p->age) {
            eldest_p = p;
        }
        curr = curr->next;
    }
    
    return eldest_p;
}

person_t *get_eldest_citizen_server_stub_unmarshal(ser_buff_t *recv_b) {
    return get_eldest_citizen(deserialize_ll_t(recv_b, deserialize_person_t_wrapper));
}

void get_eldest_citizen_server_stub_marshal(person_t *p, ser_buff_t *send_b) {
    serialize_data(send_b, (char *)p->name, 32 * sizeof(char));
    serialize_data(send_b, (char *)&p->age, sizeof(int));
    serialize_data(send_b, (char *)&p->weight, sizeof(int));
}

/* Returns sum of integers in list */
int compute_list_sum(const ll_t *l) {
    int sum = 0;
    ll_node_t *curr = l->head;
    
    while (curr) {
        sum += *((int *)curr->data);
        curr = curr->next;
    }
    
    return sum;
}

int compute_list_sum_server_stub_unmarshal(ser_buff_t *recv_b) {
    return compute_list_sum(deserialize_ll_t(recv_b, deserialize_int_wrapper));
}

void compute_list_sum_server_stub_marshal(int sum, ser_buff_t *send_b) {
    serialize_data(send_b, (char *)&sum, sizeof(int));
}

/* Splits list of integers into sublists of evens and odds */
ll_pair_t *list_splitter(const ll_t *l) {
    ll_pair_t *ll_pair = init_ll_pair();
    ll_pair->even_lst = init_list();
    ll_pair->odd_lst = init_list();
    ll_node_t *curr = l->head;
    
    while (curr) {
        int n = *((int *)curr->data);
        int *n_copy = malloc(sizeof(int));
        memcpy(n_copy, &n, sizeof(int));
        
        if (n % 2 == 1 || n % 2 == -1) {
            add_data(ll_pair->odd_lst, n_copy);
        }
        else {
            add_data(ll_pair->even_lst, n_copy);
        }
        
        curr = curr->next;
    }
    
    return ll_pair;
}

ll_pair_t *list_splitter_server_stub_unmarshal(ser_buff_t *recv_b) {
    return list_splitter(deserialize_ll_t(recv_b, deserialize_int_wrapper));
}

void list_splitter_server_stub_marshal(ll_pair_t *ll_pair, ser_buff_t *send_b) {
    serialize_ll_pair_t(ll_pair, send_b);
}

int main() {
    int opt, sockfd;
    struct sockaddr_in serv_addr, cli_addr;
      
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
      
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));
      
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
      
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    size_t len;
    len = sizeof(cli_addr);
    
    signal(SIGINT, signal_handler);  //register signal handler
    
    /* Continuously process incoming client RPC requests */
    while (flag) {
        rpc_hdr_t rpc_hdr;
        
        /* Receive RPC header from client */
        if (recvfrom(sockfd, &rpc_hdr, sizeof(rpc_hdr_t), 0, (struct sockaddr *)&cli_addr, (socklen_t *)&len) == -1) {
            printf("recv failed\n");
            return 1;
        }
        
        ser_buff_t *send_b, *recv_b;
        init_serialized_buffer_of_defined_size(&recv_b, rpc_hdr.payload_size);
        
        /* Receive serialized arguments from client */
        if (recvfrom(sockfd, recv_b->b, get_serialize_buffer_length(recv_b), 0, (struct sockaddr *) &cli_addr, (socklen_t *)&len) == -1) {
            printf("recv failed\n");
            return 1;
        }
        
        /* Invoke the appropriate RPC and serialize results to be sent back to client */
        switch (rpc_hdr.rpc_id) {
            case GET_SENIOR_CITIZENS:
                printf("getting senior citizens\n");
                ll_t *l = get_senior_citizens_server_stub_unmarshal(recv_b);
                init_serialized_buffer(&send_b);
                get_senior_citizens_server_stub_marshal(l, send_b);
                break;
            case GET_ELDEST_PERSON:
                printf("getting eldest citizen\n");
                person_t *p = get_eldest_citizen_server_stub_unmarshal(recv_b);
                init_serialized_buffer(&send_b);
                get_eldest_citizen_server_stub_marshal(p, send_b);
                break;
            case COMPUTE_LIST_SUM:
                printf("computing list sum\n");
                int sum = compute_list_sum_server_stub_unmarshal(recv_b);
                init_serialized_buffer(&send_b);
                compute_list_sum_server_stub_marshal(sum, send_b);
                break;
            case LIST_SPLITTER:
                printf("splitting list\n");
                ll_pair_t *ll_pair = list_splitter_server_stub_unmarshal(recv_b);
                init_serialized_buffer(&send_b);
                list_splitter_server_stub_marshal(ll_pair, send_b);
                break;
            default:
                printf("Invalid RPC code.\n");
                break;
        }
        
        /* Send to client as many bytes as result is */
        truncate_serialize_buffer(&send_b);
        
        /* Let client know how big result is */
        rpc_hdr.payload_size = get_serialize_buffer_length(send_b);
        
        /* Send RPC header to client */
        if (sendto(sockfd, &rpc_hdr, sizeof(rpc_hdr_t), 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr)) == -1) {
            printf("send failed\n");
            return 1;
        }
        
        /* Send results to client */
        if (sendto(sockfd, send_b->b, rpc_hdr.payload_size, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr)) == -1) {
            printf("send failed\n");
            return 1;
        }
        
        /* Free buffers */
        free_serialize_buffer(send_b);
        free_serialize_buffer(recv_b);
    }
    
    close(sockfd);
    return 0;
}
