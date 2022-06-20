#include "../inc/uch_server.h"
#define MAX_LEN 512
#define NAME_LEN 32

#include "../../frameworks/openssl/openssl/ssl.h"

t_list *users_list;


void *client_work(void *param) {
    sqlite3_create_db();

    t_client *cur = (t_client *)param;
    char login[NAME_LEN];
    char passwd[16];
    char buff_out[MAX_LEN + NAME_LEN];
    bool is_run = true;
    char choise[32];

    bool err_msg = true;
    
    // Proverka svazy
    /*char buf[6];
    SSL_read(cur->ssl, buf, 6);
    SSL_write(cur->ssl, buf, 6);*/
    // Procerka svazy

    while (err_msg) {
        // sign up or sign in
        if(SSL_read(cur->ssl, &choise, 32) <= 0) {
            free_client(&cur, &users_list);
            
            return NULL;
        }
        // login
        if(SSL_read(cur->ssl, login, NAME_LEN) <= 0 || mx_strlen(login) <  2 || mx_strlen(login) >= 32){
            free_client(&cur, &users_list);

            return NULL;
        } else{
            cur->login=mx_strtrim(login);
        }

        //passwd
        if(SSL_read(cur->ssl, passwd, 16) <= 0 || mx_strlen(passwd) <  8 || mx_strlen(passwd) > 16){
            free_client(&cur, &users_list);
            
            return NULL;

        } else{
            cur->passwd=mx_strtrim(passwd);
        }
        //DB SWITH
        if (mx_strcmp(choise, "u") == 0) {
            char *query = NULL;
            char *sql_pattern = NULL;
            t_list *list = NULL;
            sql_pattern = "SELECT EXISTS (SELECT id FROM users WHERE login=('%s'));";
            asprintf(&query, sql_pattern, cur->login);
            list = sqlite3_exec_db(query, 1);
            if (strcmp(list->data, "0") == 0) {
                //REGESTRATION TO DB
                sql_pattern = "INSERT INTO users (login, password) VALUES ('%s', '%s');";
                asprintf(&query, sql_pattern, cur->login, cur->passwd);
                sqlite3_exec_db(query, 2);
                const bool success_reg = false;
                SSL_write(cur->ssl, &success_reg, sizeof(bool));
            }
            else {
                //LOGIN ALREDAY TAKEN
                SSL_write(cur->ssl, &err_msg, sizeof(bool));
            }
        }
        else if (mx_strcmp(choise, "i") == 0 || mx_strcmp(choise, "reconnect") == 0) {
            char *query = NULL;
            char *sql_pattern = NULL;
            t_list *list = NULL;
            
            sql_pattern = "SELECT EXISTS (SELECT id FROM users WHERE login=('%s') AND password=('%s'));";
            asprintf(&query, sql_pattern, cur->login, cur->passwd);
            list = sqlite3_exec_db(query, 1);
            if (strcmp(list->data, "1") == 0) {
                //USER FOUND
                err_msg = false;
                sql_pattern = "SELECT id FROM users WHERE login=('%s') AND password=('%s');";
                asprintf(&query, sql_pattern, cur->login, cur->passwd);
                list = sqlite3_exec_db(query, 1);
            }
            else {
                //USER NOT FOUND
                SSL_write(cur->ssl, &err_msg, sizeof(bool));
            }
        }

    }

    cur->id = get_user_id(cur->login);
   
    if (mx_strcmp(choise, "i") == 0){
        SSL_write(cur->ssl, &err_msg, sizeof(bool));
        send_all_user_data(cur);
    }
    sprintf(buff_out, "%s has joined with password %s\n", cur->login, cur->passwd);
    sprintf(buff_out, "%s has joined\n", cur->login);
    //send_message(buff_out,login, NULL, true);
    char message[MAX_LEN + NAME_LEN];
    while (is_run) {
        int mes_stat = SSL_read(cur->ssl, message, MAX_LEN + NAME_LEN);

        if (mes_stat == 0 || (mx_strcmp(message, "exit") == 0)) {
            sprintf(buff_out, "%s has left\n", cur->login);
            is_run = false;
        }
        else if (mx_strcmp(message, "<users list>") == 0) {
            char buf[544] = {0};
            sprintf(buf, "<users list>");
            send_all(cur->ssl, buf, 544);
            t_list *users_l = sqlite3_exec_db("SELECT login FROM users", 1);
            
            int users_count = mx_list_size(users_l);
            SSL_write(cur->ssl, &users_count, sizeof(int));
            while (users_l) {
                char buf[20] = {0};
                sprintf(buf, "%s", users_l->data);
                send_all(cur->ssl, buf, 20);
                users_l = users_l->next;
            }
            printf("users count %d\n", users_count);
            clear_message(message, MAX_LEN + NAME_LEN);
            continue;
        }
        else if (mx_strncmp(message,"<add chat, name=",16) == 0) {
            // работа со строкой, будет всё переделано под гтк
            char *temp = message + 16;
            int len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *name = mx_strndup(message + 16, len);
            if (mx_strcmp(name, ".new_dialog") == 0) {
                mx_strdel(&name);
                name = mx_strdup(".dialog");
            }
            char **arr = NULL;
            arr = mx_strsplit(mx_strchr(message, '>') + 1, ' ');
            t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
            new_chat->users=NULL;
            
            mx_strcpy(new_chat->name, name);
            
            int i;
            mx_push_back(&new_chat->users, mx_strdup(cur->login));
            for (i = 0; arr[i]; i++) {
                mx_push_back(&new_chat->users, mx_strdup(arr[i]));
            } 
            if (mx_strcmp(name, ".dialog") == 0) {
                name = mx_strrejoin(name, " ");
                name = mx_strrejoin(name, new_chat->users->next->data);
                name = mx_strrejoin(name, " ");
                name = mx_strrejoin(name, cur->login);
            }
            new_chat->messages = NULL;
            new_chat->count_users = i + 1;
            
            //ADD TO TABLES CHAT AND MEMBERS
            char *query = NULL;
            char *sql_pattern = NULL;
            sql_pattern = "INSERT INTO chats (name, members) VALUES ('%s', %d);";
            asprintf(&query, sql_pattern, name, new_chat->count_users);
            mx_strdel(&name);
            int *data = sqlite3_exec_db(query, 2);
            int c_id = data[0];
            new_chat->id = c_id;
            new_chat->is_new = true;
            t_list *temp_list = new_chat->users;
            int admin = 1;
            while (temp_list) {
                int u_id = get_user_id(temp_list->data);
                if (admin == 1) {
                    sql_pattern = "INSERT INTO members (chat_id, user_id, admin) VALUES (%d, %d, TRUE);";
                    admin = 0;
                }
                else {
                    sql_pattern = "INSERT INTO members (chat_id, user_id) VALUES (%d, %d);";
                }
                asprintf(&query, sql_pattern, c_id, u_id);
                sqlite3_exec_db(query, 2);
                temp_list = temp_list->next;
            }

            // отправка на клиенты
            pthread_mutex_lock(&send_mutex);
            send_new_chat(new_chat);

            mx_push_back(&cur->chats, new_chat);
            pthread_mutex_unlock(&send_mutex);
            clear_message(message, MAX_LEN + NAME_LEN);
        }
        else if (mx_strncmp(message, "change chat", 11) == 0) {
            char *trim = message + 12;
            char *query = NULL;
            char *sql_pattern = NULL;
            t_list *list = NULL;
            sql_pattern = "SELECT EXISTS (SELECT name FROM chats WHERE id=('%d'));";
            asprintf(&query, sql_pattern, trim);
            list = sqlite3_exec_db(query, 1);
            if (strcmp(list->data, "1") == 0) {
                sql_pattern = "SELECT name FROM chats WHERE id=('%s');";
                asprintf(&query, sql_pattern, trim);
                list = sqlite3_exec_db(query, 1);
                char *c_name = mx_strdup(list->data);
                int c_id = get_chat_id(c_name);
                sql_pattern = "SELECT EXISTS (SELECT id FROM members WHERE chat_id=(%d) AND user_id=(%d));";
                asprintf(&query, sql_pattern, c_id, cur->id);
                list = sqlite3_exec_db(query, 1);
                if (strcmp(list->data, "1") == 0) {
                    mx_strcpy(cur->cur_chat.name, c_name);
                    cur->cur_chat.id = c_id;
                    cur->cur_chat.count_users = get_chat_members(cur->cur_chat.id);
                    cur->cur_chat.users = get_chat_users(cur->cur_chat.id);
                    
                }
                else {
                    //NOT CHAT MEMBER
                }
            }
            else {
                //NO SUCH CHAT
            }
        } 
        else if(mx_strncmp(message, "<setting avatar>", 16) == 0){
            char buf[512 + 32] = {0};
            t_avatar recv_avatar;
            recv_all(cur->ssl, buf, 512 + 32);
            recv_avatar.name = mx_strdup(buf);
            clear_message(buf, 512 + 32);
            char *path = mx_strjoin("data/avatars/", cur->login);
            sprintf(buf, "data/avatars/%s/%s", cur->login, recv_avatar.name);
            struct stat st = {0};
            if (stat("data", &st) == -1) {
                mkdir("data", 0777);
            }
            if (stat("data/avatars", &st) == -1) {
                mkdir("data/avatars", 0777);
            }
            if (stat(path, &st) == -1) {
                mkdir(path, 0777);
            }
            mx_strdel(&path);
            recv_avatar.path = mx_strdup(buf);
            clear_message(buf, 544);
            recv_image(cur->ssl, recv_avatar.path);
        
            sprintf(buf, "<image loaded>");
            send_all(cur->ssl, buf, 512 + 32); 
            clear_message(buf, 512 + 32);
            
            SSL_read(cur->ssl, &recv_avatar.scaled_w, sizeof(double));
            SSL_read(cur->ssl, &recv_avatar.scaled_h, sizeof(double));

            SSL_read(cur->ssl, &recv_avatar.x, sizeof(double));
            SSL_read(cur->ssl, &recv_avatar.y, sizeof(double));

            clear_message(buf, 512 + 32);
            char *table = NULL;
            int id = 0;
            bool is_chat = false;
            if (!mx_strstr(message, "chat_id=")) {
                table = "users";
                id = cur->id;
                sprintf(buf, "<setting avatar>");
                send_all(cur->ssl, buf, 512+32);
                send_image(cur->ssl, recv_avatar.path);
            }
            else { // <setting avatar>chat_id=
                table = "chats";
                id = mx_atoi(mx_strstr(message, "chat_id=") + 8);   
                is_chat = true;
                if (id != cur->cur_chat.id) {
                    change_chat_by_id(id, cur);
                }
            }
            sprintf(buf, "path=%s scaled_w=%f scaled_h=%f x=%f y=%f ", recv_avatar.path,recv_avatar.scaled_w, recv_avatar.scaled_h,recv_avatar.x, recv_avatar.y);
            char *query = NULL;
            char *sql_pattern = NULL;
            
            sql_pattern = "UPDATE %s SET avatar = '%s' WHERE id = %d;";
            
            asprintf(&query, sql_pattern, table, buf, id);
            sqlite3_exec_db(query, 2);

            if (is_chat) {
                clear_message(message, 544);
                sprintf(message, "<update avatar chat_id=%d>", id);
                send_message(message, cur->login, &cur->cur_chat, false);
            }
        }
        else if (mx_strncmp(mx_strtrim(message), "<setting, name=", 15) == 0) {
            char *name = NULL;
            char *surname = NULL;
            char *bio = NULL;
        
            char *temp = mx_strstr(message, "name=") + 5;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }

            name = mx_strndup(temp, len);
            temp = mx_strstr(message, "surname=") + 8;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            surname = mx_strndup(temp, len);

            bio = mx_strdup(mx_strchr(message, '>')+1);

            if (mx_strcmp(name, ".not_changed") != 0) {
                mx_strcpy(cur->name, name);
                update_user_name(cur->name, cur->id);

                // изменяешь name
            }
            if (mx_strcmp(surname, ".not_changed") != 0) {
                mx_strcpy(cur->surname, surname);
                update_user_surname(cur->surname, cur->id);
                // изменяешь surname
            }
            if (mx_strcmp(bio, ".not_changed") != 0) {
                mx_strcpy(cur->bio, bio);
                update_user_bio(cur->bio, cur->id);

                // изменяешь bio
            }
        }
        else if (mx_strncmp(message, "<setting chat_id=", 17) == 0) { //"<setting chat_id=%d, chat_name=%s>"
            char *temp = message +17;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                change_chat_by_id(chat_id, cur);
            }
            temp = mx_strstr(message, "chat_name=") + 10;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *c_name = mx_strndup(temp, len);

            char buf[544] = {0};
            sprintf(buf, "<update name chat_id=%d, chat_name=%s>", chat_id, c_name);
            update_chat_name(c_name, chat_id);
            send_message(buf, cur->login, &cur->cur_chat, false);
        }
        else if(mx_strncmp(message, "<change mute chat_id=", 21) == 0) {
            char *temp = message + 21;
            int len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);

            change_mute(chat_id, cur->id);
        }
        else if (mx_strncmp(message, "<msg, chat_id=", 14) == 0) {
            char *temp = message + 15;
            int len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                change_chat_by_id(chat_id, cur);
            }

            char *query = NULL;
            char *sql_pattern = NULL;
            char *mess = sql_protection(mx_strchr(message, '>') + 1);
            
            sql_pattern = "INSERT INTO messages (chat_id, user_id, text, time) VALUES (%d, %d, '%s', '%s');";
            asprintf(&query, sql_pattern, cur->cur_chat.id, cur->id, mess, get_time());
            int *mes_id = sqlite3_exec_db(query, 2);
            cur->cur_chat.last_mes_id = *mes_id;
            send_message(mx_strchr(message, '>') + 1, cur->login, &cur->cur_chat, true);
            clear_message(message, MAX_LEN + NAME_LEN);
        }
        else if(mx_strncmp(message, "<file chat_id=", 14) == 0) { //"<file chat_id=%d, name=%s, mode=%s>"
            char *temp = message + 14;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                change_chat_by_id(chat_id, cur);
            }

            temp = mx_strstr(temp, "name=") + 5;
            len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *name = mx_strndup(temp, len);
            char *path = mx_strjoin("data/", mx_itoa(chat_id));
            struct stat st = {0};
            if (stat(path, &st) == -1) {
                mkdir(path, 0777);
            }
            path = mx_strrejoin(path, "/");
            path = mx_strrejoin(path, name);           
            temp = mx_strstr(temp, "mode=") + 5;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }

            char *mode = mx_strndup(temp, len);
            char *query = NULL;
            char *sql_pattern = NULL;
            time_t t;
            time(&t);
            sql_pattern = "INSERT INTO messages (chat_id, user_id, text, time, type) VALUES (%d, %d, '%s', '%s', '%s');";
            asprintf(&query, sql_pattern, cur->cur_chat.id, cur->id, path, get_time(), "file");
            int *mes_id = sqlite3_exec_db(query, 2);
            char *time = mx_strndup(get_time()+11, 5);
            cur->cur_chat.last_mes_id = *mes_id;
            recv_file(cur->ssl, path, mode);

            char buf[544] = {0};
            sprintf(buf, "<file chat_id=%d, mes_id=%d, from=%s, time=%s, prev=0>%s", chat_id, *mes_id, cur->login, time, name);

            send_message(buf, cur->login, &cur->cur_chat, false);
        }
        else if (mx_strncmp(message, "<sticker chat_id=", 17) == 0) { //"<sticker chat_id=%d, sticker_num=%d>"
            char *temp = message + 17;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                change_chat_by_id(chat_id, cur);
            }

            temp = mx_strstr(message, "sticker_num=") + 12;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *s_id = mx_strndup(temp, len);
            

            char *query = NULL;
            char *sql_pattern = NULL;
            sql_pattern = "INSERT INTO messages (chat_id, user_id, text, time, type) VALUES (%d, %d, '%s', '%s', '%s');";
            asprintf(&query, sql_pattern, cur->cur_chat.id, cur->id, s_id, get_time(), "sticker");
            
            int *mes_id = sqlite3_exec_db(query, 2);
            
            cur->cur_chat.last_mes_id = *mes_id;

            char *time = mx_strndup(get_time() + 11, 5);

            char buf[544] = {0};
            sprintf(buf, "<sticker chat_id=%d, mes_id=%d, from=%s, time=%s, prev=0>%s", chat_id, *mes_id, cur->login, time, s_id);
            mx_strdel(&s_id);
            send_message(buf, cur->login, &cur->cur_chat, false);
        }
        else if (mx_strncmp(message, "<chat users avatars>", 20) == 0){
            char *id_str = message + 20;
            int chat_id = mx_atoi(id_str);
            t_list *users = get_chat_users(chat_id);

            char buf[512+32] = {0};
            sprintf(buf, "<chat users avatars>");
            send_all(cur->ssl,buf, 512+32);
            while (users) {
                if (mx_strcmp(users->data, cur->login) != 0) {
                    char *avatar_info = get_user_avatar(get_user_id(users->data));
                    t_avatar *avatar = parse_avatar_info(avatar_info);
                    
                    char *temp_str = avatar->path;
                    while (mx_strchr(temp_str,'/')) {
                        temp_str = mx_strchr(temp_str,'/') + 1;
                    }
                    char buf_name[32] = {0};
                    sprintf(buf_name, "%s", temp_str);
                    send_all(cur->ssl, buf_name, 32);
                    clear_message(buf_name, 32);
                    if (mx_strcmp(avatar->path, "default") != 0){
                        send_image(cur->ssl, avatar->path);
                        recv_all(cur->ssl, buf_name, 14);
                        SSL_write(cur->ssl, &avatar->scaled_w, sizeof(double));
                        SSL_write(cur->ssl, &avatar->scaled_h, sizeof(double));
                        SSL_write(cur->ssl, &avatar->x, sizeof(double));
                        SSL_write(cur->ssl, &avatar->y, sizeof(double));
                    }
                }

                users = users->next;
            }
            
        }
        else if (mx_strncmp(message, "<get messages chat_id=", 22) == 0) { //"<get messages chat_id=%d, last_mes=%d>"
            char *temp = message + 22;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);

            temp = mx_strstr(message, "last_mes=") + 9;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *m_id = mx_strndup(temp, len);
            int mes_id = mx_atoi(m_id);
            mx_strdel(&m_id);

            t_list *mes_list = db_messages_sender(chat_id, mes_id); //DODELAI

            char buf[512 + 32] = {0};
            while(mes_list) {
                t_message *mes_send = (t_message *)mes_list->data;
                char *time = mx_strndup(mes_send->time + 11, 5);
                if (mx_strcmp(mes_send->type, "file") == 0) {
                    char *name = mes_send->data;
                    while (mx_strchr(name, '/')) {
                        name = mx_strchr(name, '/') + 1;
                    }
                    sprintf(buf, "<file chat_id=%d, mes_id=%d, from=%s, time=%s, prev=1>%s", chat_id, mes_send->id, mes_send->sender, time, name);
                }
                else if (mx_strcmp(mes_send->type, "sticker") == 0) {
                    sprintf(buf, "<sticker chat_id=%d, mes_id=%d, from=%s, time=%s, prev=1>%s", chat_id, mes_send->id, mes_send->sender, time, mes_send->data);
                }
                else {
                    sprintf(buf, "<msg, chat_id=%d, mes_id=%d, from=%s, type=%s, time=%s, prev=1>%s", chat_id, mes_send->id, mes_send->sender, mes_send->type, time, mes_send->data);
                }
                mx_strdel(&time);
                send_all(cur->ssl, buf, 512 + 32);

                clear_message(buf, 544);
                int status = 0;
                SSL_read(cur->ssl, &status, sizeof(int));
                if (!status)
                    break;
                mes_list = mes_list->next;
            }

            sprintf(buf, "<last message>");
            send_all(cur->ssl, buf, 512 + 32);
            //send (cur->cl_socket, &count_mes, sizeof(int), 0);
        }
        else if(mx_strncmp(message, "<get user avatar>", 17) == 0) { //<get user avatar>
            char *user_name = message + 17;
            char *avatar_info = get_user_avatar(get_user_id(user_name));
            t_avatar *avatar = parse_avatar_info(avatar_info);
            char buf[544] = {0};
            sprintf(buf, "<get user avatar>");
            send_all(cur->ssl, buf, 544);
            send_avatar(avatar, cur->ssl);
        }
        else if(mx_strncmp(message, "<get chat avatar chat_id=", 25) == 0) { //"<get chat avatar chat_id=%d>"
            char *temp = message + 25;
            int len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            char *avatar_info = get_chat_avatar(chat_id);
            t_avatar *avatar = parse_avatar_info(avatar_info);

            send_avatar(avatar, cur->ssl);
        }
        else if (mx_strncmp(message, "<get file chat_id=", 18) == 0) { // "<get file chat_id=%d, mes_id=%d>"
            char *temp = message + 18;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                change_chat_by_id(chat_id, cur);
            }
            temp = mx_strstr(temp, "mes_id=") + 7;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *m_id = mx_strndup(temp, len);
            int mes_id = mx_atoi(m_id);
            mx_strdel(&m_id);

            char *path = get_message_data_by_id(mes_id, chat_id);

            char buf[544] = {0};
            sprintf(buf, "<get file>");
            send_all(cur->ssl, buf, 544);
            char *mode = "rb";
            if (mx_strstr(path, ".txt")) {
                mode = "r";
            }
            send_file(cur->ssl, path, mode);
        }
        else if (mx_strncmp(message, "<delete mes chat_id=", 20) == 0) {
            char *temp = message + 20;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                change_chat_by_id(chat_id, cur);
            }
            temp = mx_strstr(temp, "mes_id=") + 7;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *m_id = mx_strndup(temp, len);
            int mes_id = mx_atoi(m_id);
            mx_strdel(&m_id);
            
            db_delete_message(mes_id);

            send_message(message, cur->login, &cur->cur_chat, false);
        }
        else if (mx_strncmp(message, "<edit msg, chat_id=", 19) == 0) { // "<edit msg, chat_id=%d, mes_id=%d>%s"
            char *temp = message + 19;
            int len = 0;
            while (*(temp + len) != ',') {
                len++;
            }
            char *c_id = mx_strndup(temp, len);
            int chat_id = mx_atoi(c_id);
            mx_strdel(&c_id);
            if (chat_id != cur->cur_chat.id) {
                change_chat_by_id(chat_id, cur);
            }
            temp = mx_strstr(temp, "mes_id=") + 7;
            len = 0;
            while (*(temp + len) != '>') {
                len++;
            }
            char *m_id = mx_strndup(temp, len);
            int mes_id = mx_atoi(m_id);
            mx_strdel(&m_id);
            
            message_changer(mes_id, mx_strchr(message, '>') + 1);

            send_message(message, cur->login, &cur->cur_chat, false);
        }
        else if (mx_strncmp(message, "<get last mes id>", 17) == 0) { // "<get last mes id>"
            send_all(cur->ssl, message, 544);
            int mes_id = get_message_max_id(cur->id, cur->cur_chat.id);
            SSL_write(cur->ssl, &mes_id, sizeof(int));
        }
        else if (mes_stat > 0) {
		    if(cur->cur_chat.id != 0){
                send_message(message, cur->login, &cur->cur_chat, true);
            }
            else {
                send_message(message, cur->login, NULL, true);
            }
            clear_message(message, MAX_LEN + NAME_LEN);
        }
        
    }

    // отключение клиента от сервера
    close_connection(cur->ssl);
    free_client(&cur, &users_list);
    return NULL;
}

t_client *create_new_client(const struct sockaddr_in adr, int client_fd, int *client_id) {
    t_client *new_client = (t_client *)malloc(sizeof(t_client));
    new_client->adr = adr;
    new_client->cl_socket = client_fd;
    new_client->login = NULL;
    new_client->passwd = NULL;
    //new_client->id = client_id;
    new_client->scaled_for_chat = NULL;
    new_client->chats = NULL;
    t_chat init_chat = {0};
    new_client->cur_chat = init_chat;

    (*client_id)++;
    mx_push_back(&users_list, new_client);
    return new_client;
}

void client_work_wrapper(SSL_CTX *context, int client_fd, pthread_t *thread, t_client *new_client) {
    // threadin
    SSL *ssl;
    ssl = SSL_new(context);             //get new SSL state
    SSL_set_fd(ssl, client_fd);         //set connection socket to SSL state

    if (SSL_accept(ssl) < 0) {
        ERR_print_errors_fp(stderr);
        close_connection(ssl);
        return;
    }

    new_client->ssl = ssl;
    // Certificate check (delete? anyway client has no certs)
    //certificate_ckeck(ssl);

    // nu thread
    pthread_create(thread, NULL, client_work, new_client);
}


int main_server(int argc, char *argv[]);

int main (int argc, char *argv[]) {
    if (argc != 2) {
        mx_printerr("usage: ./uchat_server <port>\n");
        return -1;
    }
    pid_t process_id = 0;
    pid_t sid = 0;
    
    process_id = fork();
    if (process_id < 0) {
        exit(1);
    }
    if (process_id > 0) {
        printf("server pid = %d\n", process_id);
        exit(0);
    }
    umask(0);
    sid = setsid();
    
    if (sid < 0)
        exit(1);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    main_server(argc, argv);

    return 0;
}

int main_server(int argc, char *argv[]) {
    printf("%s\n", SSLeay_version(SSLEAY_VERSION));
    
    //signal(SIGPIPE, SIG_IGN);
    if (argc != 2) {
        mx_printerr("usage: ./uchat_server <port>\n");
        return -1;
    }

    //  SSLing
    SSL_CTX *context;
    SSL_library_init();
    context = CTX_initialize_server();

    // creaating certificate and pkey
    EVP_PKEY *pkey = create_key();
    X509 *x509 = create_X509(pkey);
    write_certs(pkey, x509);
    load_certs(context, "certificate.pem", "PEM_privatekey.pem");
    // for client serving
    pthread_t thread;
    users_list = NULL;
    pthread_mutex_init(&send_mutex, NULL);

    int client_id = 0;
    int client_fd;
    struct sockaddr_in adr = {0};
    socklen_t adrlen = sizeof(adr);
    int serv_fd = open_server_connection(mx_atoi(argv[1]), &adr, adrlen);
    while (1) {
        // accept client connection
        client_fd = accept(serv_fd, (struct sockaddr*) &adr, &adrlen);
        printf("SSL: client accepted\n");
        printf("SSL: Connection: %s:%d\n",inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));

        // handle client
        t_client *new_client = create_new_client(adr, client_fd, &client_id);
        client_work_wrapper(context, client_fd, &thread, new_client);
        sleep(1);
    }

    close(client_fd);
    close_server(pkey, x509, context);
    return 0;
}

char *sql_protection(char *message) {
    int problem = 0;
    for(int i = 0; message[i]; i++) {
        if (message[i] == '\'') {
            problem++;
        }
    }
    char *temp = mx_strnew(mx_strlen(message) + problem);
    int i = 0;
    int j = 0;
    while (message[i]) {
        if (message[i] =='\'' && message[i-1] !='\'') {
            temp[j] = message[i];
            temp[j+1] = message[i];
            j++;
        }
        else {
            temp[j] = message[i];
        }
        j++;
        i++;
    }
    return temp;
}

char *get_time() {
    time_t t;
    time(&t);
    return ctime(&t);
}
