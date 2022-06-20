#include "../inc/uch_client.h"

t_client cur_client;
pthread_mutex_t cl_mutex;


gboolean add_msg(gpointer data) {
    t_message *message = data;
    char *total_msg = message->data;
    GtkWidget *incoming_msg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *incoming_msg = gtk_label_new(total_msg);

    bool is_sender = false;
    if (mx_strcmp(message->sender, cur_client.login) != 0){
        gtk_widget_set_halign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_START);
        //gtk_widget_set_margin_start(incoming_msg_box, 5);
    }   
    else {
        gtk_widget_set_halign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_END);
        gtk_widget_set_valign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_END);
        //ыgtk_widget_set_margin_end(incoming_msg_box, 5);
        
        is_sender = true;
    }
    gtk_widget_set_margin_bottom(incoming_msg_box, 5);
   

   //для цсс--------------------------------------------------------------
    if (!is_sender)
        gtk_widget_set_name(GTK_WIDGET(incoming_msg), "incoming-message");
    else 
        gtk_widget_set_name(GTK_WIDGET(incoming_msg), "message");
    load_css_main(t_screen.provider, incoming_msg);
   //для цсс--------------------------------------------------------------


    gtk_label_set_wrap(GTK_LABEL(incoming_msg), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(incoming_msg), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_max_width_chars(GTK_LABEL(incoming_msg), 50);
    gtk_label_set_selectable(GTK_LABEL(incoming_msg), FALSE);
    GtkWidget *User_logo = NULL;


    if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0) 
    {
        if (!is_sender) 
        {
            t_list *avatars = cur_client.cur_chat.users_avatars;
            t_list *user_list = cur_client.cur_chat.users;
            while (mx_strcmp(user_list->data, message->sender) != 0 && user_list) {
                if (mx_strcmp(user_list->data, cur_client.login) == 0) {
                    user_list = user_list->next;
                    continue;
                }
                avatars = avatars->next;
                user_list = user_list->next;
            }

            if (avatars)
            {
                t_avatar *draw = avatars->data;
                if (mx_strcmp (draw->name, "default") == 0) 
                {
                    draw = &t_main.default_avatar;
                }
                User_logo = get_circle_widget_from_png_avatar(draw, 45, 45, false);
            }
        }

        else {
            User_logo = get_circle_widget_from_png_avatar(&cur_client.avatar, 45, 45, false);
        }
    }

    GtkWidget *message_box = NULL;
    if(mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0)
    {   
        message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(message_box, 5);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(message_box, 5);
        }
        gtk_widget_set_margin_bottom(message_box, 5);
        gtk_box_set_spacing(GTK_BOX(message_box), 10);

        GtkWidget* message_content = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(message_content), 1);
        gtk_grid_set_column_spacing(GTK_GRID(message_content), 5);

        GtkWidget *user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(user_name_box, 15);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(user_name_box, 15);
        }

        GtkWidget *user_name = gtk_label_new(message->sender);
        gtk_widget_set_name(user_name, "user_name_chat");
        load_css_main(t_screen.provider, user_name);
        gtk_box_append(GTK_BOX(user_name_box), user_name);

        incoming_msg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(incoming_msg_box), incoming_msg);

        GtkWidget *user_logo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_halign(GTK_WIDGET(user_logo_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_logo_box), GTK_ALIGN_CENTER);
        GtkWidget *user_logo = User_logo;
        gtk_box_append(GTK_BOX(user_logo_box), user_logo);

        GtkWidget *user_action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        GtkWidget *user_action = gtk_label_new("edited");
        gtk_widget_set_name(user_action, "user_action");
        load_css_main(t_screen.provider, user_action);
        GtkWidget *user_action_time = gtk_label_new(message->time);
        gtk_widget_set_name(user_action_time, "user_action");
        load_css_main(t_screen.provider, user_action_time);

        gtk_box_append(GTK_BOX(user_action_box), user_action);
        gtk_box_append(GTK_BOX(user_action_box), user_action_time);
        if (mx_strcmp(message->type, "text_edited") != 0)
            gtk_widget_hide(user_action);

        if(is_sender)
        {
            gtk_box_prepend(GTK_BOX(incoming_msg_box), user_action_box);
            //gtk_grid_attach(GTK_GRID(message_content), user_name_box, 1, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(message_content), incoming_msg_box, 1, 2, 1, 1);
            gtk_grid_attach(GTK_GRID(message_content), user_logo_box, 2, 1, 1, 2);
        
            GtkGesture *gesture = gtk_gesture_click_new();
            gtk_gesture_set_state(gesture, GTK_EVENT_SEQUENCE_CLAIMED);
            gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture), 3);
            GtkWidget **arr = (GtkWidget **)malloc(2*sizeof(GtkWidget *));
            arr[0] = incoming_msg;
            arr[1] = incoming_msg_box;
            int *mes_id = (int *)malloc(sizeof(int));
            *mes_id = message->id;
            t_list *gesture_data = NULL;
            mx_push_back(&gesture_data, arr);
            mx_push_back(&gesture_data, mes_id);
            g_signal_connect_after(gesture, "pressed", G_CALLBACK(show_message_menu), gesture_data);
            gtk_widget_add_controller(incoming_msg_box, GTK_EVENT_CONTROLLER(gesture));
        }
        else
        {
            gtk_box_append(GTK_BOX(incoming_msg_box), user_action_box);
            gtk_grid_attach(GTK_GRID(message_content), user_logo_box, 1, 1, 1, 2);
            gtk_grid_attach(GTK_GRID(message_content), user_name_box, 2, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(message_content), incoming_msg_box, 2, 2, 1, 1);            
        }

        gtk_box_append(GTK_BOX(message_box), message_content);
        //gtk_box_append(GTK_BOX(incoming_msg_box), User_logo);
    }
    else
    {
        message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(message_box, 5);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(message_box, 5);
        }
        gtk_widget_set_margin_bottom(message_box, 5);
        gtk_box_set_spacing(GTK_BOX(message_box), 10);

        incoming_msg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(incoming_msg_box), GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(incoming_msg_box), incoming_msg);

        GtkWidget *user_action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        GtkWidget *user_action = gtk_label_new("edited");
        gtk_widget_set_name(user_action, "user_action");
        load_css_main(t_screen.provider, user_action);
        GtkWidget *user_action_time = gtk_label_new(message->time);
        gtk_widget_set_name(user_action_time, "user_action");
        load_css_main(t_screen.provider, user_action_time);

        gtk_box_append(GTK_BOX(user_action_box), user_action);
        gtk_box_append(GTK_BOX(user_action_box), user_action_time);
        if (mx_strcmp(message->type, "text_edited") != 0)
            gtk_widget_hide(user_action);

        if(is_sender)
        {
            gtk_box_prepend(GTK_BOX(incoming_msg_box), user_action_box);
            GtkGesture *gesture = gtk_gesture_click_new();
            gtk_gesture_set_state(gesture, GTK_EVENT_SEQUENCE_CLAIMED);
            gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture), 3);
            GtkWidget **arr = (GtkWidget **)malloc(2*sizeof(GtkWidget *));
            arr[0] = incoming_msg;
            arr[1] = incoming_msg_box;
            int *mes_id = (int *)malloc(sizeof(int));
            *mes_id = message->id;
            t_list *gesture_data = NULL;
            mx_push_back(&gesture_data, arr);
            mx_push_back(&gesture_data, mes_id);
            g_signal_connect_after(gesture, "pressed", G_CALLBACK(show_message_menu), gesture_data);
            gtk_widget_add_controller(incoming_msg_box, GTK_EVENT_CONTROLLER(gesture));
        }
        else
        {
            gtk_box_append(GTK_BOX(incoming_msg_box), user_action_box);         
        }
        gtk_box_append(GTK_BOX(message_box), incoming_msg_box);
        //gtk_box_append(GTK_BOX(incoming_msg_box), User_logo);
    }

    if (!message->prev) {
        gtk_box_append(GTK_BOX(t_main.scroll_box_right), message_box);
        mx_push_front(&t_main.message_widgets_list, incoming_msg);
    }
    else {
        gtk_box_prepend(GTK_BOX(t_main.scroll_box_right), message_box);
        mx_push_back(&t_main.message_widgets_list, incoming_msg);
    }
    t_main.last_mes = incoming_msg_box;
    
    pthread_mutex_unlock(&cl_mutex);

    return FALSE;
}

gboolean add_file_msg(gpointer data) {
    t_file_mes *file_mes = (t_file_mes *)data;

    GtkWidget *incoming_file_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *incoming_file_name = gtk_label_new(file_mes->name);
    bool is_sender = false;
    if (mx_strcmp(file_mes->sender, cur_client.login) != 0){
        gtk_widget_set_halign(GTK_WIDGET(incoming_file_box), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(incoming_file_box), GTK_ALIGN_START);
    }   
    else {
        gtk_widget_set_halign(GTK_WIDGET(incoming_file_box), GTK_ALIGN_END);
        gtk_widget_set_valign(GTK_WIDGET(incoming_file_box), GTK_ALIGN_END);
        
        is_sender = true;
    }
    gtk_widget_set_margin_end(incoming_file_box, 5);
    gtk_widget_set_margin_bottom(incoming_file_box, 5);
   
    if (!is_sender)
        gtk_widget_set_name(GTK_WIDGET(incoming_file_name), "incoming-message");
    else 
        gtk_widget_set_name(GTK_WIDGET(incoming_file_name), "message");
    load_css_main(t_screen.provider, incoming_file_name);
    
    gtk_label_set_selectable(GTK_LABEL(incoming_file_name), FALSE);
    GtkWidget *User_logo = NULL;
    if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0) {
        if (!is_sender) {
            t_list *avatars = cur_client.cur_chat.users_avatars;
            t_list *user_list = cur_client.cur_chat.users;
            while (mx_strcmp(user_list->data, file_mes->sender) != 0 && user_list) {
                if (mx_strcmp(user_list->data, cur_client.login) == 0) {
                    user_list = user_list->next;
                    continue;
                }
                avatars = avatars->next;
                user_list = user_list->next;
            }

            if (avatars) {
                t_avatar *draw = avatars->data;
                if (mx_strcmp (draw->name, "default") == 0) {
                    draw = &t_main.default_avatar;
                }
                User_logo = get_circle_widget_from_png_avatar(draw, 45, 45, false);
                //gtk_box_append(GTK_BOX(incoming_file_box), User_logo);
            }
        }
        else {
            User_logo = get_circle_widget_current_user_avatar();
        }
    }

    //gtk_box_append(GTK_BOX(incoming_file_box), incoming_file_name);

    GtkWidget *file_icon = get_icon_from_filename(file_mes->name); 
    gtk_widget_set_size_request(file_icon, 45, 45);

//icon
    GtkGesture *gesture = gtk_gesture_click_new();
    gtk_gesture_set_state(gesture, GTK_EVENT_SEQUENCE_CLAIMED);

    g_signal_connect_after(gesture, "pressed", G_CALLBACK(choise_dir), file_mes);
    gtk_widget_add_controller(file_icon, GTK_EVENT_CONTROLLER(gesture));
//icon

    GtkWidget *file_box = NULL;
    if(mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0)
    {   
        file_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(file_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(file_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(file_box, 5);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(file_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(file_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(file_box, 5);
        }
        gtk_widget_set_margin_bottom(file_box, 5);
        gtk_box_set_spacing(GTK_BOX(file_box), 10);

        GtkWidget* message_content = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(message_content), 1);
        gtk_grid_set_column_spacing(GTK_GRID(message_content), 5);

        GtkWidget *user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(user_name_box, 15);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(user_name_box, 15);
        }

        GtkWidget *user_name = gtk_label_new(file_mes->sender);
        gtk_widget_set_name(user_name, "user_name_chat");
        load_css_main(t_screen.provider, user_name);
        gtk_box_append(GTK_BOX(user_name_box), user_name);

        incoming_file_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(incoming_file_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(incoming_file_box), GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(incoming_file_box), incoming_file_name);
        GtkWidget *incoming_ico_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_box_append(GTK_BOX(incoming_ico_box), file_icon);

        GtkWidget *user_logo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_halign(GTK_WIDGET(user_logo_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_logo_box), GTK_ALIGN_CENTER);
        GtkWidget *user_logo = User_logo;
        gtk_box_append(GTK_BOX(user_logo_box), user_logo);

        GtkWidget *user_action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        GtkWidget *user_action_time = gtk_label_new(file_mes->time);
        gtk_widget_set_name(user_action_time, "user_action");
        load_css_main(t_screen.provider, user_action_time);

        gtk_box_append(GTK_BOX(user_action_box), user_action_time);

        if(is_sender)
        {
            gtk_box_prepend(GTK_BOX(incoming_file_box), incoming_ico_box);
            gtk_box_prepend(GTK_BOX(incoming_file_box), user_action_box);
            gtk_grid_attach(GTK_GRID(message_content), incoming_file_box, 1, 2, 1, 1);
            gtk_grid_attach(GTK_GRID(message_content), user_logo_box, 2, 1, 1, 2);
        }
        else
        {
            gtk_box_append(GTK_BOX(incoming_file_box), incoming_ico_box);
            gtk_box_append(GTK_BOX(incoming_file_box), user_action_box);
            gtk_grid_attach(GTK_GRID(message_content), user_logo_box, 1, 1, 1, 2);
            gtk_grid_attach(GTK_GRID(message_content), user_name_box, 2, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(message_content), incoming_file_box, 2, 2, 1, 1);            
        }

        gtk_box_append(GTK_BOX(file_box), message_content);
        //gtk_box_append(GTK_BOX(incoming_msg_box), User_logo);
    }
    else
    {
        file_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(file_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(file_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(file_box, 5);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(file_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(file_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(file_box, 5);
        }
        gtk_widget_set_margin_bottom(file_box, 5);
        gtk_box_set_spacing(GTK_BOX(file_box), 10);

        incoming_file_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(incoming_file_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(incoming_file_box), GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(incoming_file_box), incoming_file_name);
        GtkWidget *incoming_ico_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_box_append(GTK_BOX(incoming_ico_box), file_icon);

        GtkWidget *user_action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        GtkWidget *user_action_time = gtk_label_new(file_mes->time);
        gtk_widget_set_name(user_action_time, "user_action");
        load_css_main(t_screen.provider, user_action_time);

        gtk_box_append(GTK_BOX(user_action_box), user_action_time);

        if(is_sender)
        {
            gtk_box_prepend(GTK_BOX(incoming_file_box), incoming_ico_box);
            gtk_box_prepend(GTK_BOX(incoming_file_box), user_action_box);
        }
        else
        {
            gtk_box_append(GTK_BOX(incoming_file_box), incoming_ico_box);
            gtk_box_append(GTK_BOX(incoming_file_box), user_action_box);         
        }
        gtk_box_append(GTK_BOX(file_box), incoming_file_box);
        //gtk_box_append(GTK_BOX(incoming_msg_box), User_logo);
    }

    if (!file_mes->prev) {
        gtk_box_append(GTK_BOX(t_main.scroll_box_right), file_box);
        //mx_push_front(&t_main.message_widgets_list, incoming_msg);
    }
    else {
        gtk_box_prepend(GTK_BOX(t_main.scroll_box_right), file_box);
        //mx_push_back(&t_main.message_widgets_list, incoming_msg);
    }
    //t_main.last_mes = incoming_msg_box;
    
    pthread_mutex_unlock(&cl_mutex);

    return FALSE;
}

gboolean add_sticker_msg(gpointer data) {
    t_sticker *sticker_mes = (t_sticker *)data;

    GtkWidget *incoming_sticker_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *incoming_sticker = gtk_image_new_from_file(mx_strjoin(mx_strjoin("client/media/stickers/", mx_itoa (sticker_mes->sticker_num)), ".png"));
    gtk_widget_set_size_request(incoming_sticker, 130, 130);
    bool is_sender = false;
    if (mx_strcmp(sticker_mes->sender, cur_client.login) != 0){
        gtk_widget_set_halign(GTK_WIDGET(incoming_sticker_box), GTK_ALIGN_START);
        gtk_widget_set_valign(GTK_WIDGET(incoming_sticker_box), GTK_ALIGN_START);
        //gtk_widget_set_margin_start(incoming_sticker_box, 5);
    }   
    else {
        gtk_widget_set_halign(GTK_WIDGET(incoming_sticker_box), GTK_ALIGN_END);
        gtk_widget_set_valign(GTK_WIDGET(incoming_sticker_box), GTK_ALIGN_END);
        //ыgtk_widget_set_margin_end(incoming_sticker_box, 5);
        
        is_sender = true;
    }
    gtk_widget_set_margin_bottom(incoming_sticker_box, 5);

    GtkWidget *User_logo = NULL;

    if (mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0) 
    {
        if (!is_sender) 
        {
            t_list *avatars = cur_client.cur_chat.users_avatars;
            t_list *user_list = cur_client.cur_chat.users;
            while (mx_strcmp(user_list->data, sticker_mes->sender) != 0 && user_list) {
                if (mx_strcmp(user_list->data, cur_client.login) == 0) {
                    user_list = user_list->next;
                    continue;
                }
                avatars = avatars->next;
                user_list = user_list->next;
            }

            if (avatars)
            {
                t_avatar *draw = avatars->data;
                if (mx_strcmp (draw->name, "default") == 0) 
                {
                    draw = &t_main.default_avatar;
                }
                User_logo = get_circle_widget_from_png_avatar(draw, 45, 45, false);
            }
        }

        else {
            User_logo = get_circle_widget_from_png_avatar(&cur_client.avatar, 45, 45, false);
        }
    }

    GtkWidget *message_box = NULL;
    if(mx_strncmp(cur_client.cur_chat.name, ".dialog", 7) != 0)
    {   
        message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(message_box, 5);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(message_box, 5);
        }
        gtk_widget_set_margin_bottom(message_box, 5);
        gtk_box_set_spacing(GTK_BOX(message_box), 10);

        GtkWidget* message_content = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(message_content), 1);
        gtk_grid_set_column_spacing(GTK_GRID(message_content), 5);

        GtkWidget *user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(user_name_box, 15);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(user_name_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(user_name_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(user_name_box, 15);
        }

        GtkWidget *user_name = gtk_label_new(sticker_mes->sender);
        gtk_widget_set_name(user_name, "user_name_chat");
        load_css_main(t_screen.provider, user_name);
        gtk_box_append(GTK_BOX(user_name_box), user_name);

        incoming_sticker_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(incoming_sticker_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(incoming_sticker_box), GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(incoming_sticker_box), incoming_sticker);

        GtkWidget *user_logo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_halign(GTK_WIDGET(user_logo_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_logo_box), GTK_ALIGN_CENTER);
        GtkWidget *user_logo = User_logo;
        gtk_box_append(GTK_BOX(user_logo_box), user_logo);

        GtkWidget *user_action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        
        GtkWidget *user_action_time = gtk_label_new(sticker_mes->time);
        gtk_widget_set_name(user_action_time, "user_action");
        load_css_main(t_screen.provider, user_action_time);

        gtk_box_append(GTK_BOX(user_action_box), user_action_time);

        if(is_sender)
        {
            gtk_box_prepend(GTK_BOX(incoming_sticker_box), user_action_box);
            //gtk_grid_attach(GTK_GRID(message_content), user_name_box, 1, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(message_content), incoming_sticker_box, 1, 2, 1, 1);
            gtk_grid_attach(GTK_GRID(message_content), user_logo_box, 2, 1, 1, 2);
        }
        else
        {
            gtk_box_append(GTK_BOX(incoming_sticker_box), user_action_box);
            gtk_grid_attach(GTK_GRID(message_content), user_logo_box, 1, 1, 1, 2);
            gtk_grid_attach(GTK_GRID(message_content), user_name_box, 2, 1, 1, 1);
            gtk_grid_attach(GTK_GRID(message_content), incoming_sticker_box, 2, 2, 1, 1);            
        }

        gtk_box_append(GTK_BOX(message_box), message_content);
        //gtk_box_append(GTK_BOX(incoming_sticker_box), User_logo);
    }
    else
    {
        message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        if(is_sender) 
        {
            gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_END);
            gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_END);
            gtk_widget_set_margin_end(message_box, 5);
        }
        else
        {
            gtk_widget_set_halign(GTK_WIDGET(message_box), GTK_ALIGN_START);
            gtk_widget_set_valign(GTK_WIDGET(message_box), GTK_ALIGN_START);
            gtk_widget_set_margin_start(message_box, 5);
        }
        gtk_widget_set_margin_bottom(message_box, 5);
        gtk_box_set_spacing(GTK_BOX(message_box), 10);

        incoming_sticker_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(incoming_sticker_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(incoming_sticker_box), GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(incoming_sticker_box), incoming_sticker);

        GtkWidget *user_action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
        gtk_widget_set_halign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(user_action_box), GTK_ALIGN_CENTER);

        GtkWidget *user_action_time = gtk_label_new(sticker_mes->time);
        gtk_widget_set_name(user_action_time, "user_action");
        load_css_main(t_screen.provider, user_action_time);

        gtk_box_append(GTK_BOX(user_action_box), user_action_time);
        //if (mx_strcmp(sticker_mes->type, "text_edited") != 0)
        //    gtk_widget_hide(user_action);

        if(is_sender)
        {
            gtk_box_prepend(GTK_BOX(incoming_sticker_box), user_action_box);
        }
        else
        {
            gtk_box_append(GTK_BOX(incoming_sticker_box), user_action_box);         
        }
        gtk_box_append(GTK_BOX(message_box), incoming_sticker_box);
        //gtk_box_append(GTK_BOX(incoming_sticker_box), User_logo);
    }

    if (!sticker_mes->prev) {
        gtk_box_append(GTK_BOX(t_main.scroll_box_right), message_box);
        mx_push_front(&t_main.message_widgets_list, incoming_sticker);
    }
    else {
        gtk_box_prepend(GTK_BOX(t_main.scroll_box_right), message_box);
        mx_push_back(&t_main.message_widgets_list, incoming_sticker);
    }
    t_main.last_mes = incoming_sticker_box;
    
    pthread_mutex_unlock(&cl_mutex);

    return FALSE;
}

void *rec_func() {
    while(!t_main.loaded) {
        usleep(500);
    }

    char message[512 + 32] = {0};
    while (t_main.is_run) {
        SSL *fd = cur_client.ssl;
		int receive = swiftchat_recv(fd, message, 512 + 32);

        if (receive > 0) {
            if(mx_strcmp(mx_strtrim(message), "<add chat>") == 0) {
                t_chat *new_chat = (t_chat *)malloc(sizeof(t_chat));
                new_chat->is_new = true;
                new_chat->messages = NULL;
                new_chat->users = NULL;

                char buf_name[256] = {0};
                receive = swiftchat_recv(fd, buf_name, 256);
                while (receive < 0) {
                    receive = swiftchat_recv(fd, buf_name, 256);
                }
                mx_strcpy(new_chat->name, buf_name);
                receive = swiftchat_recv(fd, &new_chat->id, sizeof(int));
                while (receive < 0) {
                    receive = swiftchat_recv(fd, &new_chat->id, sizeof(int));
                }
                receive = swiftchat_recv(fd, &new_chat->count_users, sizeof(int));
                while (receive < 0) {
                    receive = swiftchat_recv(fd, &new_chat->count_users, sizeof(int));
                }
                for (int i = 0; i < new_chat->count_users; i++) {
                    char buf[32] = {0};
                    receive = swiftchat_recv(fd, buf, 32);
                    while (receive < 0) {
                        receive = swiftchat_recv(fd, buf, 32);
                    }
                    mx_push_back(&new_chat->users,mx_strdup(buf));
                    clear_message(buf, 32);
                }
                /////// аватарка чата
                if (mx_strncmp(new_chat->name, ".dialog", 7) !=0) {
                    t_main.loaded_avatar = (t_avatar *)malloc(sizeof(t_avatar));
                    get_avatar(t_main.loaded_avatar);
                    new_chat->avatar=*t_main.loaded_avatar;
                    if (mx_strcmp(new_chat->avatar.name, "default") == 0) {
                        if (mx_strncmp(new_chat->name, ".dialog", 7) != 0)
                            new_chat->avatar = t_main.default_group_avatar;
                        else
                            new_chat->avatar = t_main.default_avatar;
                    }
                    new_chat->is_avatar = true;
                }
                else {
                    new_chat->is_avatar = false;
                }

                //////
                mx_push_back(&cur_client.chats, new_chat);
                if (!cur_client.sender_new_chat){
                    g_idle_add(add_chat_node, new_chat);
                }
                else {
                    cur_client.sender_new_chat = false;
                }
                cur_client.chat_count++;
                t_main.loaded = true;

                fflush(stdout);
            }
            else if (mx_strcmp(mx_strtrim(message), "<users list>") == 0) {
                pthread_mutex_lock(&cl_mutex);
                t_list *users_list = NULL;
                int count_users = 0;
                swiftchat_recv(cur_client.ssl, &count_users, sizeof(int));
                for (int i = 0; i < count_users; i++) {
                    char buf[20] = {0};
                    swiftchat_recv(cur_client.ssl, buf, 20);
                    mx_push_back(&users_list, mx_strtrim(buf));
                }
                t_main.search_users_list = users_list;
                pthread_mutex_unlock(&cl_mutex);
                t_main.loaded = true;

            }
            else if (mx_strncmp(message, "<msg, chat_id=", 14) == 0){ // "<msg, chat_id=%d, mes_id=%d, from=%s, prev=1>%s"
                char *temp = message + 14;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                int chat_id = mx_atoi(c_id);                        // ид чата, в который надо вставить сообщение
                (void)chat_id; // избавляюсь от unused variable
                mx_strdel(&c_id);
                temp = mx_strstr(message, "mes_id=") + 7;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *mes_id = mx_strndup(temp, len);
                int message_id = mx_atoi(mes_id);
                mx_strdel(&c_id);
                temp = mx_strstr(message, "from=") + 5;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *sender = mx_strndup(temp, len);              // отправитель
                temp = mx_strstr(message, "prev=") + 5;
                bool prev = *temp == '0' ? false : true; 

                char *time = mx_strndup (mx_strstr(message, "time=") + 5, 5);

                char *total_msg = mx_strdup(mx_strchr(message, '>') + 1);     // сообщение
                                                                   // время надо получить локально на клиенте

                temp = mx_strstr(message, "type=") + 5;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *type = mx_strndup(temp, len);

                if (cur_client.cur_chat.id == chat_id) {
                    t_message *mes = (t_message *)malloc(sizeof(t_message));
                    mes->c_id = chat_id;
                    mes->prev = prev;
                    mes->id = message_id;
                    
                    mx_strcpy(mes->sender, sender);
                    mx_strcpy(mes->data, total_msg);
                    mx_strcpy(mes->time, time);
                    mx_strcpy(mes->type, type);

                    pthread_mutex_lock(&cl_mutex);
                    g_idle_add(add_msg, mes);
                    pthread_mutex_lock(&cl_mutex);
                    if (prev) {
                        cur_client.cur_chat.last_mes_id = mes->id;
                        int status = 1;
                        swiftchat_send(cur_client.ssl, &status, sizeof(int));
                        mx_push_back(&cur_client.cur_chat.messages, mes);
                    }
                    else {
                        mx_push_front(&cur_client.cur_chat.messages, mes);
                    }
                    
                    pthread_mutex_unlock(&cl_mutex);
                }
                else if (prev) {
                    int status = 0;
                    swiftchat_send(cur_client.ssl, &status, sizeof(int));        
                }
                else if (cur_client.cur_chat.id != chat_id){
                    t_list *temp_widgets = t_main.chat_nodes_info;
                    t_list *temp_ch = cur_client.chats;
                    
                    while (temp_ch)
                    {
                        if (((t_chat *)temp_ch->data)->id == chat_id && ((t_chat *)temp_ch->data)->notify == false)
                        {
                            t_note.notification_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                            gtk_widget_set_valign(t_note.notification_box,GTK_ALIGN_CENTER);
                            gtk_widget_set_halign(t_note.notification_box,GTK_ALIGN_START);
                            gtk_widget_set_margin_start(t_note.notification_box, 100);
                            GtkWidget *notification = NULL;
                            if (!((t_chat *)temp_ch->data)->mute)
                                notification = gtk_image_new_from_file("client/media/unmuted.png");
                            else
                                notification = gtk_image_new_from_file("client/media/muted.png");
                            gtk_widget_set_size_request(notification, 10, 10);

                            gtk_box_append(GTK_BOX(t_note.notification_box), notification);
                            gtk_box_append(GTK_BOX(temp_widgets->data), t_note.notification_box);
                            ((t_chat *)temp_ch->data)->is_enter = false;
                            ((t_chat *)temp_ch->data)->notify = true;
                        }
                        temp_widgets = temp_widgets->next;
                        temp_ch = temp_ch->next;
                    }

                } 
                
                if (!prev || t_main.scroll_mes) {           
                    pthread_t display_thread = NULL;
                    pthread_create(&display_thread, NULL, scroll_func, NULL);  
                }
        
                fflush(stdout);
            }
            else if (mx_strncmp(message, "<file chat_id=", 14) == 0) { 
                char *temp = message + 14;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                int chat_id = mx_atoi(c_id);                        // ид чата, в который надо вставить сообщение
                (void)chat_id; // избавляюсь от unused variable
                mx_strdel(&c_id);
                temp = mx_strstr(message, "mes_id=") + 7;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *mes_id = mx_strndup(temp, len);
                int message_id = mx_atoi(mes_id);
                mx_strdel(&c_id);
                temp = mx_strstr(message, "from=") + 5;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *sender = mx_strndup(temp, len);              // отправитель
                temp = mx_strstr(message, "prev=") + 5;
                bool prev = *temp == '0' ? false : true; 
                char *time = mx_strndup (mx_strstr(message, "time=") + 5, 5);
                char *name = mx_strchr(message, '>') + 1;

                if (cur_client.cur_chat.id == chat_id) {
                    t_file_mes *new_file = (t_file_mes *)malloc(sizeof(t_file_mes));
                    new_file->name = mx_strdup(name);
                    new_file->sender = mx_strdup(sender);
                    new_file->data = NULL;
                    new_file->time = time;
                    new_file->prev = prev;
                    new_file->id = message_id;
                    pthread_mutex_lock(&cl_mutex);
                    g_idle_add(add_file_msg, new_file);
                    pthread_mutex_lock(&cl_mutex);
                    if (prev) {
                        cur_client.cur_chat.last_mes_id = new_file->id;
                        int status = 1;
                        swiftchat_send(cur_client.ssl, &status, sizeof(int));
                        //mx_push_back(&cur_client.cur_chat.messages, mes);
                    }
                    
                    pthread_mutex_unlock(&cl_mutex);
                }
                else if (prev) {
                    int status = 0;
                    swiftchat_send(cur_client.ssl, &status, sizeof(int));        
                }
                /*else 
                {
                    t_list *temp_widgets = t_main.chat_nodes_info;
                    t_list *temp_ch = cur_client.chats;

                    while (temp_ch)
                    {
                        if (((t_chat *)temp_ch->data)->id == chat_id)
                        {
                            t_note.notification_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                            gtk_widget_set_valign(t_note.notification_box,GTK_ALIGN_CENTER);
                            gtk_widget_set_halign(t_note.notification_box,GTK_ALIGN_START);
                            GtkWidget *notification = gtk_image_new_from_file("client/media/unmuted.png");
                            gtk_box_append(GTK_BOX(t_note.notification_box), notification);
                            gtk_box_append(GTK_BOX(temp_widgets->data), t_note.notification_box);
                            ((t_chat *)temp_ch->data)->is_enter = false;
                        }
                        temp_widgets = temp_widgets->next;
                        temp_ch = temp_ch->next;
                    }

                }*/
                
                if (!prev || t_main.scroll_mes) {           
                    pthread_t display_thread = NULL;
                    pthread_create(&display_thread, NULL, scroll_func, NULL);  
                }
        
                fflush(stdout);                
            }
            else if (mx_strncmp(message, "<sticker chat_id=", 17) == 0) { // "<sticker chat_id=%d, mes_id=%d, from=%s, prev=0>%s"
                char *temp = message + 17;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                int chat_id = mx_atoi(c_id);                        // ид чата, в который надо вставить сообщение
                (void)chat_id; // избавляюсь от unused variable
                mx_strdel(&c_id);
                temp = mx_strstr(message, "mes_id=") + 7;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *mes_id = mx_strndup(temp, len);
                int message_id = mx_atoi(mes_id);
                mx_strdel(&c_id);
                temp = mx_strstr(message, "from=") + 5;
                len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *time = mx_strndup (mx_strstr(message, "time=") + 5, 5);
                char *sender = mx_strndup(temp, len);              // отправитель
                temp = mx_strstr(message, "prev=") + 5;
                bool prev = *temp == '0' ? false : true; 

                char *sticker_num = mx_strchr(message, '>') + 1;

                if (cur_client.cur_chat.id == chat_id) {
                    t_sticker *new_sticker = (t_sticker *)malloc(sizeof(t_sticker));
                    new_sticker->sticker_num = mx_atoi(sticker_num);
                    new_sticker->sender = sender;
                    new_sticker->time = time;
                    new_sticker->data = NULL;
                    new_sticker->prev = prev;
                    new_sticker->id = message_id;
                    pthread_mutex_lock(&cl_mutex);
                    g_idle_add(add_sticker_msg, new_sticker);  // вот тут отрисовка
                    pthread_mutex_lock(&cl_mutex);
                    if (prev) {
                        cur_client.cur_chat.last_mes_id = new_sticker->id;
                        int status = 1;
                        swiftchat_send(cur_client.ssl, &status, sizeof(int));
                        //mx_push_back(&cur_client.cur_chat.messages, mes);
                    }
                    
                    pthread_mutex_unlock(&cl_mutex);
                }
                else if (prev) {
                    int status = 0;
                    swiftchat_send(cur_client.ssl, &status, sizeof(int));        
                }
                
                if (!prev || t_main.scroll_mes) {           
                    pthread_t display_thread = NULL;
                    pthread_create(&display_thread, NULL, scroll_func, NULL);  
                }
        

                fflush(stdout);
            }
            else if(mx_strncmp(message, "<get file>", 10) == 0) { // "<get file chat_id=%d, mes_id=%d>"
                char *path = mx_strjoin(t_main.choosed_dir, "/");
                path = mx_strrejoin(path, t_main.choosed_file->name);

                char *mode = "wb";
                if (mx_strstr(path, ".txt")) {
                    mode = "w";
                }

                pthread_mutex_lock(&cl_mutex);
                recv_file(cur_client.ssl, path, mode);
                t_main.choosed_file = NULL;
                t_main.choosed_dir = NULL;
                pthread_mutex_unlock(&cl_mutex);
            }
            else if(mx_strncmp(message, "<last message>", 14) == 0) {
                if (!t_main.first_load_mes){
                    pthread_t display_thread = NULL;
                    pthread_create(&display_thread, NULL, save_scroll_func, NULL);
                }
                else {
                    pthread_t display_thread = NULL;
                    pthread_create(&display_thread, NULL, scroll_func, NULL);  
                    t_main.first_load_mes = false;
                }  
            } // "<get last mes id>"
            else if (mx_strncmp(message, "<get last mes id>", 17) == 0) {
                swiftchat_recv(cur_client.ssl, &t_main.send_mes_id, sizeof(int));
                t_main.loaded = true;
            }
            else if(mx_strncmp(message, "<delete mes chat_id=", 20) == 0) { // "<delete mes chat_id=%d, mes_id=%d>"
                char *temp = message + 20;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);

                int chat_id = mx_atoi(c_id);
                mx_strdel(&c_id);
                if (cur_client.cur_chat.id == chat_id) {
                    // найти и удалить
                    temp = mx_strstr(temp, "mes_id=") + 7;
                    len = 0;
                    while (*(temp + len) != '>') {
                        len++;
                    }
                    char *m_id = mx_strndup(temp, len);
                    int mes_id = mx_atoi(m_id);
                    mx_strdel(&m_id);

                    t_list *temp_mes = cur_client.cur_chat.messages;
                    t_list *temp_widgets = t_main.message_widgets_list;
                    t_list *prev_m = NULL;
                    t_list *prev_w = NULL;

                    while(temp_mes) {
                        t_message *mes = (t_message *)temp_mes->data;
                        if (mes->id == mes_id) {
                            break;
                        }
                        prev_m = temp_mes;
                        prev_w = temp_widgets;

                        temp_widgets = temp_widgets->next;
                        temp_mes = temp_mes->next;
                    }

                    GtkWidget *box = gtk_widget_get_parent(GTK_WIDGET (temp_widgets->data));
                    gtk_widget_hide(box);
    
                    if (temp_mes && prev_m){
                        prev_m->next = temp_mes->next;
                        free(temp_mes->data);
                        free(temp_mes);
                        temp_mes = NULL;
                    }
                    if (temp_widgets && prev_w) {
                        prev_w->next = temp_widgets->next;
                        free(temp_widgets);
                        temp_widgets = NULL;
                    }
                    if (!prev_w) {
                        mx_pop_front(&t_main.message_widgets_list);
                    }
                    if(!prev_m) {
                        mx_pop_front(&cur_client.cur_chat.messages);

                    }

                } 
            }
            if(mx_strncmp(message, "<edit msg, chat_id=", 19) == 0) { // "<edit msg, chat_id=%d, mes_id=%d>%s"
                char *temp = message + 19;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                int chat_id = mx_atoi(c_id);
                mx_strdel(&c_id);
                if (cur_client.cur_chat.id == chat_id) {
                    // найти и изменить
                    temp = mx_strstr(temp, "mes_id=") + 7;
                    len = 0;
                    while (*(temp + len) != '>') {
                        len++;
                    }
                    char *m_id = mx_strndup(temp, len);
                    int mes_id = mx_atoi(m_id);
                    mx_strdel(&m_id);

                    t_list *temp_mes = cur_client.cur_chat.messages;
                    t_list *temp_widgets = t_main.message_widgets_list;

                    while(temp_mes) {
                        t_message *mes = (t_message *)temp_mes->data;
                        if (mes->id == mes_id) {
                            break;
                        }

                        temp_widgets = temp_widgets->next;
                        temp_mes = temp_mes->next;
                    }

                    const char *text = mx_strchr(message, '>') + 1;
                    gtk_label_set_text(GTK_LABEL (temp_widgets->data), text);
                    GtkWidget *parent = gtk_widget_get_parent(temp_widgets->data);
                    GtkWidget *edited_label = gtk_widget_get_first_child(gtk_widget_get_last_child(parent));
                    if (!gtk_widget_get_visible(edited_label)) {
                        gtk_widget_show(edited_label);
                    }
                } 
            }
            else if(mx_strcmp(mx_strtrim(message), "<setting avatar>") == 0) {
                char buf[544] = {0};
                sprintf(buf, "client_data/%s", cur_client.avatar.name);

                recv_image(cur_client.ssl, buf);
                if (malloc_size (cur_client.avatar.path))
                    mx_strdel(&cur_client.avatar.path);
                cur_client.avatar.path = mx_strdup(buf);

                t_main.loaded = true;
            }
            else if(mx_strncmp(mx_strtrim(message), "<get user avatar>",17) == 0) {
                t_main.loaded_avatar = (t_avatar *)malloc(sizeof(t_avatar));
                get_avatar(t_main.loaded_avatar);

                if (mx_strcmp(t_main.loaded_avatar->name, "default") == 0) {
                    *t_main.loaded_avatar = t_main.default_avatar;
                }
                t_main.loaded = true;
            }
            else if(mx_strncmp(mx_strtrim(message), "<update avatar chat_id=",23) == 0) {
                char *temp = mx_strstr(message, "chat_id=") + 8;
                int len = 0;
                while (*(temp + len) != '>') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                int chat_id = mx_atoi(c_id);
                mx_strdel(&c_id);

                char buf[544] = {0};
                sprintf(buf, "<get chat avatar chat_id=%d>",chat_id);
                swiftchat_send(cur_client.ssl, buf, 544);
                t_main.loaded_avatar = (t_avatar *)malloc(sizeof(t_avatar));
                get_avatar(t_main.loaded_avatar);
                if (mx_strcmp(t_main.loaded_avatar->name, "default") == 0) {
                    *t_main.loaded_avatar = t_main.default_group_avatar;
                }
                
                t_list *temp_widgets = t_main.chat_nodes_info;
                t_list *temp_ch = cur_client.chats;

                while (temp_ch) {
                    if (((t_chat *)temp_ch->data)->id == chat_id) {
                        break;
                    }
                    temp_widgets = temp_widgets->next;
                    temp_ch = temp_ch->next;
                }

                ((t_chat *)temp_ch->data)->avatar = *t_main.loaded_avatar;
                gtk_box_remove(GTK_BOX (temp_widgets->data), gtk_widget_get_first_child(GTK_WIDGET (temp_widgets->data)));

                GtkWidget *chat_image = get_circle_widget_from_png_avatar(t_main.loaded_avatar, 57, 57, true);

                gtk_box_prepend(GTK_BOX (temp_widgets->data), chat_image);

                if (cur_client.cur_chat.id == chat_id) {
                    GtkWidget *header_bar_box = gtk_widget_get_first_child(t_main.right_panel);
                    GtkWidget *header_bar_left = gtk_grid_get_child_at(GTK_GRID(header_bar_box), 0, 0);
                                     
                    GtkWidget *photo_box = gtk_widget_get_first_child(header_bar_left);
                    gtk_widget_hide(photo_box);
                    photo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
                    GtkWidget *photo =  NULL;
                    photo = get_circle_widget_from_png_avatar(t_main.loaded_avatar, 50, 50, true);
                    
                    gtk_box_append(GTK_BOX(photo_box), photo);
                    gtk_box_prepend(GTK_BOX(header_bar_left), photo_box);

                }
            }
            else if(mx_strncmp(message, "<update name chat_id=",21) == 0) { // "<setting chat_id=%d, chat_name=%s>"
                char *temp = mx_strstr(message, "chat_id=") + 8;
                int len = 0;
                while (*(temp + len) != ',') {
                    len++;
                }
                char *c_id = mx_strndup(temp, len);
                int chat_id = mx_atoi(c_id);
                mx_strdel(&c_id);

                temp = mx_strstr(message, "chat_name=") + 10;
                len = 0;
                while (*(temp + len) != '>') {
                    len++;
                }
                char *c_name = mx_strndup(temp, len);  
                
                t_list *temp_widgets = t_main.chat_nodes_info;
                t_list *temp_ch = cur_client.chats;

                while (temp_ch) {
                    if (((t_chat *)temp_ch->data)->id == chat_id) {
                        break;
                    }
                    temp_widgets = temp_widgets->next;
                    temp_ch = temp_ch->next;
                }

                mx_strcpy(((t_chat *)temp_ch->data)->name, c_name);
                mx_strcpy(cur_client.cur_chat.name, c_name);
                gtk_label_set_text(GTK_LABEL (gtk_widget_get_last_child (temp_widgets->data)), c_name);
                if (cur_client.cur_chat.id == chat_id) {
                    GtkWidget *header_bar_box = gtk_widget_get_first_child(t_main.right_panel);
                    GtkWidget *header_bar_left = gtk_grid_get_child_at(GTK_GRID(header_bar_box), 0, 0);
                    GtkWidget *info_box = gtk_widget_get_last_child(header_bar_left);
                    gtk_widget_hide(info_box);
                    info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
                    gtk_widget_set_halign(GTK_WIDGET(info_box), GTK_ALIGN_START);
                    gtk_widget_set_valign(GTK_WIDGET(info_box), GTK_ALIGN_CENTER);
                    gtk_widget_set_margin_start(info_box, 10);
                    GtkWidget *group_or_user_name = NULL;
                   
                    group_or_user_name = gtk_label_new(cur_client.cur_chat.name);
                    
                    GtkWidget *group_or_user_name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                    gtk_widget_set_halign(GTK_WIDGET(info_box), GTK_ALIGN_START);
                    gtk_widget_set_name(GTK_WIDGET(group_or_user_name), "chat_name_inside_label");
                    load_css_main(t_screen.provider, group_or_user_name);
                    gtk_box_append(GTK_BOX(group_or_user_name_box), group_or_user_name);
                    GtkWidget *status = gtk_label_new("last seen 13 minutes ago");
                    gtk_widget_set_name(GTK_WIDGET(status), "last_online");
                    load_css_main(t_screen.provider, status);
                    gtk_box_append(GTK_BOX(info_box), group_or_user_name_box);
                    gtk_box_append(GTK_BOX(info_box), status);

                    //gtk_box_append(GTK_BOX(header_bar_left), photo_box);
                    gtk_box_append(GTK_BOX(header_bar_left), info_box);

                }
            }
            else if(mx_strcmp(mx_strtrim(message), "<image loaded>") == 0) {
                t_main.loaded = true;
            }
            else if (mx_strcmp(mx_strtrim(message), "<chat users avatars>") == 0) {
                cur_client.cur_chat.users_avatars = get_chat_users_avatars(&cur_client.cur_chat);
                t_main.loaded = true;
            }
        }  
        
		clear_message(message, 512);
    }
    return NULL;

}
static void load_css() {
    t_screen.provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(t_screen.provider,"client/themes/dark.css");
}

 void send_login(GtkWidget *widget, gpointer data) {
     if (!t_main.connected) {
        return;
    }
    (void)widget;
    GtkWidget **entry_field = (GtkWidget **)data;
    GtkEntryBuffer *login_field_buf = gtk_entry_get_buffer(GTK_ENTRY (entry_field[0]));
    GtkEntryBuffer *password_field_buf = gtk_entry_get_buffer(GTK_ENTRY (entry_field[1]));
    
    int login_len = gtk_entry_buffer_get_length(login_field_buf);
    int password_len = gtk_entry_buffer_get_length(password_field_buf);

    if (login_len < 6 || login_len > 20) {
        gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessageLogin), "LOGIN MUST CONTAIN MORE THAN 3 CHARACTERS");
        gtk_widget_show(t_auth.ErrorMessageLogin);
        return;
    }

    if (password_len < 8 || password_len > 16) {
        gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessageLogin), "PASSWORD MUST CONTAIN MORE THAN 8 CHARACTERS");
        gtk_widget_show(t_auth.ErrorMessageLogin);
        return;
    }
    
    cur_client.login = mx_strdup(gtk_entry_buffer_get_text(login_field_buf));
    cur_client.passwd = mx_strdup(gtk_entry_buffer_get_text(password_field_buf));
    int status = check_auth_input(cur_client.login);
    switch(status) {
        case 0:
            break;
        default:{
            mx_strdel(&cur_client.login);
            mx_strdel(&cur_client.passwd);   
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessageLogin), "INCORRECT LOGIN OR PASSWORD");
            gtk_widget_show(t_auth.ErrorMessageLogin); 
            return;
        }
    }

    status = check_auth_input(cur_client.passwd);
    switch(status) {
        case 0:
            break;
        default:{
            mx_strdel(&cur_client.login);
            mx_strdel(&cur_client.passwd);
            gtk_label_set_label (GTK_LABEL(t_auth.ErrorMessageLogin), "INCORRECT LOGIN OR PASSWORD");
            gtk_widget_show(t_auth.ErrorMessageLogin); 
            return;
        }
    }

    char message[32] = {0};
    // Отправка данных для авторизации на сервер
    sprintf(message, "i");
    swiftchat_send(cur_client.ssl, message, 32);
    clear_message(message, 32);
    sprintf(message, "%s", cur_client.login);
    swiftchat_send(cur_client.ssl, message, 32);
    clear_message(message, 32);
    sprintf(message, "%s", cur_client.passwd);
    swiftchat_send(cur_client.ssl, message, 16);


    bool err_aut = true;
    swiftchat_recv(cur_client.ssl, &err_aut, sizeof(bool)); // Ожидание ответа от сервера об успешности входа или регистрации

    if (err_aut) {
        mx_strdel(&cur_client.login);
        mx_strdel(&cur_client.passwd);
        gtk_label_set_label(GTK_LABEL(t_auth.ErrorMessageLogin), "INCORRECT LOGIN OR PASSWORD");
        gtk_widget_show(t_auth.ErrorMessageLogin);
        mx_printerr("login err\n");
        return;
    }

    get_all_user_data();
    create_user_db(cur_client);

    GtkWidget *child = gtk_window_get_child(GTK_WINDOW (t_screen.main_window));
    
    gtk_widget_unparent(child);
    // можем заходить в чат, вход успешен
    chat_show_main_screen(t_screen.main_window);
}

static void activate(GtkApplication *application) {
    t_screen.main_window = gtk_application_window_new (application);
    gtk_window_set_title (GTK_WINDOW ( t_screen.main_window), "Swiftchat");
    gtk_window_set_default_size (GTK_WINDOW ( t_screen.main_window), 1200, 760);
    gtk_window_set_resizable (GTK_WINDOW ( t_screen.main_window), FALSE);
    load_css();
    chat_show_auth_screen();
    chat_decorate_headerbar();
    gtk_widget_show(t_screen.main_window);
}


int main(int argc, char *argv[]) {
    (void)argv;
    if (argc != 3) {
        mx_printerr("usage: ./uchat <server IP> <port>\n");
        return -1;
    }
    t_client cur = {
        .login = NULL,
        .passwd = NULL,
        .chat_count = 0,
        .chats = NULL,
        .ssl = NULL,
        .avatar = {
            .orig_w = 512,
            .orig_h = 512,
            .scaled_w = 300,
            .scaled_h = 300,
            .image = NULL,
            .path = "client/media/default_user.png",
            .name = "default_user.png", 
            .x = 200,
            .y = 200
        },
    };
    t_avatar default_avatar = {
        .orig_w = 512,
        .orig_h = 512,
        .scaled_w = 300,
        .scaled_h = 300,
        .image = NULL,
        .path = "client/media/default_user.png",
        .name = "default_user.png", 
        .x = 200,
        .y = 200
    };
    t_main.default_avatar = default_avatar;
    t_avatar default_group_avatar = {
        .orig_w = 512,
        .orig_h = 512,
        .scaled_w = 300,
        .scaled_h = 300,
        .image = NULL,
        .path = "client/media/default_groupchat.png",
        .name = "default_groupchat.png", 
        .x = 200,
        .y = 200
    };
    t_main.default_group_avatar = default_group_avatar;
    cur_client = cur;
    struct stat st = {0};
    if (stat("client_data", &st) == -1) {
        mkdir("client_data", 0777);
    }
    t_main.is_run = true;
    t_main.ip = mx_strdup(argv[1]);
    t_main.port = mx_atoi(argv[2]);
    //      =====   SSLing    =====
    t_main.context = CTX_initialize_client();
    //cur_client.ssl = SSL_new(t_main.context);
    open_ssl_connection();
    t_main.loaded = false;
    pthread_t rec_th;
    pthread_mutex_init(&cl_mutex, NULL);
    pthread_create(&rec_th, NULL, rec_func, &cur_client.serv_fd);

    GtkApplication *application;
    gint status;
    application = gtk_application_new("my.first.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(application), FALSE, NULL);

    pthread_join(rec_th, NULL);

    close(cur_client.serv_fd);
    SSL_free(cur_client.ssl);
    SSL_CTX_free(t_main.context);

    return 0;//status;
}
