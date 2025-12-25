#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

struct Education {
    char level[20];              // Degree / PUC / School
    char institution[100];       // University / College / School Name
    int year;
    union {
        float cgpa;
        float percentage;
    };
};

struct Student {
    char name[50];
    int age;
    char gender[10];
    char phone[20];
    char email[50];
    char address[100];
    char objective[300];
    struct Education education[3];  // 0: Degree (CGPA), 1: PUC (Percentage), 2: School (Percentage)
    char skills[200];
    char projects[300];
    char achievements[300];
    char languages[100];
    char hobbies[100];
};

struct AuthState {
    gboolean is_logged_in;
    char username[50];
};

struct AuthState auth_state = {FALSE, ""};

void removeNewline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

// Global widgets for accessing in callbacks
typedef struct {
    GtkWidget *main_window;
    GtkWidget *auth_window;
    
    // Resume builder widgets
    GtkWidget *name_entry;
    GtkWidget *age_entry;
    GtkWidget *gender_entry;
    GtkWidget *phone_entry;
    GtkWidget *email_entry;
    GtkWidget *address_entry;
    GtkWidget *objective_entry;
    
    // Education entries
    GtkWidget *degree_institution;
    GtkWidget *degree_year;
    GtkWidget *degree_cgpa;
    
    GtkWidget *puc_institution;
    GtkWidget *puc_year;
    GtkWidget *puc_percentage;
    
    GtkWidget *school_institution;
    GtkWidget *school_year;
    GtkWidget *school_percentage;
    
    GtkWidget *skills_entry;
    GtkWidget *projects_entry;
    GtkWidget *achievements_entry;
    GtkWidget *languages_entry;
    GtkWidget *hobbies_entry;
    
    // Login/signup related widgets
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *login_button;
    GtkWidget *signup_button;
    GtkWidget *auth_status_label;
} AppWidgets;

// Forward declarations
void create_login_interface(AppWidgets *widgets);
void create_resume_interface(AppWidgets *widgets);
void login_user(GtkWidget *button, AppWidgets *widgets);
void signup_user(GtkWidget *button, AppWidgets *widgets);
void generate_resume(GtkWidget *button, AppWidgets *widgets);
void logout_user(GtkWidget *button, AppWidgets *widgets);
GtkWidget* create_compact_labeled_entry(GtkWidget *grid, const char *label_text, int row);

// Create and return a labeled entry
GtkWidget* create_labeled_entry(GtkWidget *grid, const char *label_text, int row) {
    GtkWidget *label = gtk_label_new(label_text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    
    GtkWidget *entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, row, 1, 1);
    
    return entry;
}

// Function to generate resume from GUI data
void generate_resume(GtkWidget *button, AppWidgets *widgets) {
    struct Student s;
    
    // Get data from GUI widgets
    strncpy(s.name, gtk_entry_get_text(GTK_ENTRY(widgets->name_entry)), sizeof(s.name)-1);
    s.age = atoi(gtk_entry_get_text(GTK_ENTRY(widgets->age_entry)));
    strncpy(s.gender, gtk_entry_get_text(GTK_ENTRY(widgets->gender_entry)), sizeof(s.gender)-1);
    strncpy(s.phone, gtk_entry_get_text(GTK_ENTRY(widgets->phone_entry)), sizeof(s.phone)-1);
    strncpy(s.email, gtk_entry_get_text(GTK_ENTRY(widgets->email_entry)), sizeof(s.email)-1);
    strncpy(s.address, gtk_entry_get_text(GTK_ENTRY(widgets->address_entry)), sizeof(s.address)-1);
    strncpy(s.objective, gtk_entry_get_text(GTK_ENTRY(widgets->objective_entry)), sizeof(s.objective)-1);
    
    // Education details
    strcpy(s.education[0].level, "Degree");
    strncpy(s.education[0].institution, gtk_entry_get_text(GTK_ENTRY(widgets->degree_institution)), sizeof(s.education[0].institution)-1);
    s.education[0].year = atoi(gtk_entry_get_text(GTK_ENTRY(widgets->degree_year)));
    s.education[0].cgpa = atof(gtk_entry_get_text(GTK_ENTRY(widgets->degree_cgpa)));
    
    strcpy(s.education[1].level, "PUC");
    strncpy(s.education[1].institution, gtk_entry_get_text(GTK_ENTRY(widgets->puc_institution)), sizeof(s.education[1].institution)-1);
    s.education[1].year = atoi(gtk_entry_get_text(GTK_ENTRY(widgets->puc_year)));
    s.education[1].percentage = atof(gtk_entry_get_text(GTK_ENTRY(widgets->puc_percentage)));
    
    strcpy(s.education[2].level, "School");
    strncpy(s.education[2].institution, gtk_entry_get_text(GTK_ENTRY(widgets->school_institution)), sizeof(s.education[2].institution)-1);
    s.education[2].year = atoi(gtk_entry_get_text(GTK_ENTRY(widgets->school_year)));
    s.education[2].percentage = atof(gtk_entry_get_text(GTK_ENTRY(widgets->school_percentage)));
    
    strncpy(s.skills, gtk_entry_get_text(GTK_ENTRY(widgets->skills_entry)), sizeof(s.skills)-1);
    strncpy(s.projects, gtk_entry_get_text(GTK_ENTRY(widgets->projects_entry)), sizeof(s.projects)-1);
    strncpy(s.achievements, gtk_entry_get_text(GTK_ENTRY(widgets->achievements_entry)), sizeof(s.achievements)-1);
    strncpy(s.languages, gtk_entry_get_text(GTK_ENTRY(widgets->languages_entry)), sizeof(s.languages)-1);
    strncpy(s.hobbies, gtk_entry_get_text(GTK_ENTRY(widgets->hobbies_entry)), sizeof(s.hobbies)-1);
    
    // Create filename with username
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_resume.txt", auth_state.username);
    
    // Write to file
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->main_window),
                                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_MESSAGE_ERROR,
                                                  GTK_BUTTONS_CLOSE,
                                                  "Failed to create resume file.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Write formatted resume to file
    fprintf(f, "========================================\n");
    fprintf(f, "               RESUME                   \n");
    fprintf(f, "========================================\n\n");
    
    fprintf(f, "PERSONAL INFORMATION:\n");
    fprintf(f, "---------------------\n");
    fprintf(f, "Name: %s\n", s.name);
    fprintf(f, "Age: %d\n", s.age);
    fprintf(f, "Gender: %s\n", s.gender);
    fprintf(f, "Phone: %s\n", s.phone);
    fprintf(f, "Email: %s\n", s.email);
    fprintf(f, "Address: %s\n\n", s.address);
    
    fprintf(f, "CAREER OBJECTIVE:\n");
    fprintf(f, "-----------------\n");
    fprintf(f, "%s\n\n", s.objective);
    
    fprintf(f, "EDUCATION:\n");
    fprintf(f, "----------\n");
    for (int i = 0; i < 3; i++) {
        if (strlen(s.education[i].institution) > 0) {
            fprintf(f, "%s: %s (%d)\n", s.education[i].level, s.education[i].institution, s.education[i].year);
            if (i == 0) {
                fprintf(f, "CGPA: %.2f\n", s.education[i].cgpa);
            } else {
                fprintf(f, "Percentage: %.2f%%\n", s.education[i].percentage);
            }
            fprintf(f, "\n");
        }
    }
    
    fprintf(f, "TECHNICAL SKILLS:\n");
    fprintf(f, "-----------------\n");
    fprintf(f, "%s\n\n", s.skills);
    
    fprintf(f, "PROJECTS:\n");
    fprintf(f, "---------\n");
    fprintf(f, "%s\n\n", s.projects);
    
    fprintf(f, "ACHIEVEMENTS:\n");
    fprintf(f, "-------------\n");
    fprintf(f, "%s\n\n", s.achievements);
    
    fprintf(f, "LANGUAGES KNOWN:\n");
    fprintf(f, "----------------\n");
    fprintf(f, "%s\n\n", s.languages);
    
    fprintf(f, "HOBBIES:\n");
    fprintf(f, "--------\n");
    fprintf(f, "%s\n\n", s.hobbies);
    
    fprintf(f, "========================================\n");
    
    fclose(f);
    
    // Show success message
    char success_msg[200];
    snprintf(success_msg, sizeof(success_msg), "✅ Resume saved successfully as '%s'", filename);
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->main_window),
                                              GTK_DIALOG_DESTROY_WITH_PARENT,
                                              GTK_MESSAGE_INFO,
                                              GTK_BUTTONS_OK,
                                              "%s", success_msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Login callback function
void login_user(GtkWidget *button, AppWidgets *widgets) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(widgets->username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(widgets->password_entry));
    
    // Simple authentication check
    if (strlen(username) > 0 && strlen(password) >= 4) {
        auth_state.is_logged_in = TRUE;
        strncpy(auth_state.username, username, sizeof(auth_state.username)-1);
        
        // Show success message
        gtk_label_set_text(GTK_LABEL(widgets->auth_status_label), 
                          "✅ Login successful! Opening resume builder...");
        
        // Hide auth window and show resume builder
        gtk_widget_hide(widgets->auth_window);
        create_resume_interface(widgets);
        
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->auth_status_label), 
                          "❌ Login failed. Username required and password must be at least 4 characters.");
    }
}

// Signup callback function
void signup_user(GtkWidget *button, AppWidgets *widgets) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(widgets->username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(widgets->password_entry));
    
    // Simple signup validation
    if (strlen(username) > 0 && strlen(password) >= 4) {
        auth_state.is_logged_in = TRUE;
        strncpy(auth_state.username, username, sizeof(auth_state.username)-1);
        
        // Show success message
        gtk_label_set_text(GTK_LABEL(widgets->auth_status_label), 
                          "✅ Account created successfully! Opening resume builder...");
        
        // Hide auth window and show resume builder
        gtk_widget_hide(widgets->auth_window);
        create_resume_interface(widgets);
        
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->auth_status_label), 
                          "❌ Signup failed. Username required and password must be at least 4 characters.");
    }
}

// Logout callback function
void logout_user(GtkWidget *button, AppWidgets *widgets) {
    auth_state.is_logged_in = FALSE;
    strcpy(auth_state.username, "");
    
    // Clear all entry fields
    gtk_entry_set_text(GTK_ENTRY(widgets->username_entry), "");
    gtk_entry_set_text(GTK_ENTRY(widgets->password_entry), "");
    gtk_label_set_text(GTK_LABEL(widgets->auth_status_label), "Please login or signup to continue");
    
    // Hide resume builder and show auth window
    if (widgets->main_window) {
        gtk_widget_hide(widgets->main_window);
    }
    gtk_widget_show_all(widgets->auth_window);
}

// Create authentication interface
void create_auth_interface(AppWidgets *widgets) {
    GtkWidget *window, *grid, *title_label, *button_box;
    
    // Create authentication window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Resume Builder - Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    widgets->auth_window = window;
    
    // Create grid for layout
    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_container_add(GTK_CONTAINER(window), grid);
    
    // Title
    title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), 
                        "<span size='x-large' weight='bold'>Resume Builder</span>\n"
                        "<span size='medium'>Please login or create an account</span>");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 2, 1);
    
    // Username field
    widgets->username_entry = create_labeled_entry(grid, "Username:", 1);
    gtk_widget_set_size_request(widgets->username_entry, 200, -1);
    
    // Password field
    widgets->password_entry = create_labeled_entry(grid, "Password:", 2);
    gtk_entry_set_visibility(GTK_ENTRY(widgets->password_entry), FALSE);
    gtk_widget_set_size_request(widgets->password_entry, 200, -1);
    
    // Button box for login and signup buttons
    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    
    // Login button
    widgets->login_button = gtk_button_new_with_label("Login");
    gtk_widget_set_size_request(widgets->login_button, 100, 40);
    g_signal_connect(widgets->login_button, "clicked", G_CALLBACK(login_user), widgets);
    gtk_box_pack_start(GTK_BOX(button_box), widgets->login_button, FALSE, FALSE, 0);
    
    // Signup button
    widgets->signup_button = gtk_button_new_with_label("Sign Up");
    gtk_widget_set_size_request(widgets->signup_button, 100, 40);
    g_signal_connect(widgets->signup_button, "clicked", G_CALLBACK(signup_user), widgets);
    gtk_box_pack_start(GTK_BOX(button_box), widgets->signup_button, FALSE, FALSE, 0);
    
    gtk_grid_attach(GTK_GRID(grid), button_box, 0, 3, 2, 1);
    
    // Status label
    widgets->auth_status_label = gtk_label_new("Please login or signup to continue");
    gtk_widget_set_halign(widgets->auth_status_label, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), widgets->auth_status_label, 0, 4, 2, 1);
    
    // Show all widgets
    gtk_widget_show_all(window);
}

// Create resume builder interface with reduced spacing
void create_resume_interface(AppWidgets *widgets) {
    GtkWidget *window, *grid, *button, *scrolled_window;
    int row = 0;
    
    // Create window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Resume Builder");
    gtk_window_set_default_size(GTK_WINDOW(window), 580, 650);  // Reduced size
    gtk_container_set_border_width(GTK_CONTAINER(window), 8);   // Reduced border
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    widgets->main_window = window;
    
    // Create scrollable area
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    
    // Create grid for form layout with reduced spacing
    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);   // Reduced column spacing
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);      // Reduced row spacing
    gtk_container_add(GTK_CONTAINER(scrolled_window), grid);
    
    // Welcome message with username
    char welcome_text[100];
    snprintf(welcome_text, sizeof(welcome_text), "Welcome %s! Create your resume below:", auth_state.username);
    GtkWidget *welcome = gtk_label_new(welcome_text);
    gtk_widget_set_halign(welcome, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(welcome, 5);  // Small margin below welcome
    gtk_grid_attach(GTK_GRID(grid), welcome, 0, row++, 2, 1);
    
    // Personal Information Section with compact header
    GtkWidget *personal_header = gtk_label_new("Personal Information");
    gtk_widget_set_halign(personal_header, GTK_ALIGN_START);
    gtk_widget_set_margin_top(personal_header, 2);
    gtk_widget_set_margin_bottom(personal_header, 2);
    gtk_grid_attach(GTK_GRID(grid), personal_header, 0, row++, 2, 1);
    
    // Create form fields with compact layout
    widgets->name_entry = create_compact_labeled_entry(grid, "Name:", row++);
    widgets->age_entry = create_compact_labeled_entry(grid, "Age:", row++);
    widgets->gender_entry = create_compact_labeled_entry(grid, "Gender:", row++);
    widgets->phone_entry = create_compact_labeled_entry(grid, "Phone:", row++);
    widgets->email_entry = create_compact_labeled_entry(grid, "Email:", row++);
    widgets->address_entry = create_compact_labeled_entry(grid, "Address:", row++);
    
    widgets->objective_entry = create_compact_labeled_entry(grid, "Career Objective:", row++);
    gtk_widget_set_size_request(widgets->objective_entry, 400, 40); // Reduced height
    
    // Education section header - compact
    GtkWidget *edu_label = gtk_label_new("Education Details");
    gtk_widget_set_halign(edu_label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(edu_label, 3);
    gtk_widget_set_margin_bottom(edu_label, 2);
    gtk_grid_attach(GTK_GRID(grid), edu_label, 0, row++, 2, 1);
    
    // Degree - compact layout
    widgets->degree_institution = create_compact_labeled_entry(grid, "Degree Institution:", row++);
    widgets->degree_year = create_compact_labeled_entry(grid, "Degree Year:", row++);
    widgets->degree_cgpa = create_compact_labeled_entry(grid, "CGPA:", row++);
    
    // PUC - compact layout
    widgets->puc_institution = create_compact_labeled_entry(grid, "PUC Institution:", row++);
    widgets->puc_year = create_compact_labeled_entry(grid, "PUC Year:", row++);
    widgets->puc_percentage = create_compact_labeled_entry(grid, "PUC Percentage:", row++);
    
    // School - compact layout
    widgets->school_institution = create_compact_labeled_entry(grid, "School Name:", row++);
    widgets->school_year = create_compact_labeled_entry(grid, "School Year:", row++);
    widgets->school_percentage = create_compact_labeled_entry(grid, "School Percentage:", row++);
    
    // Additional Details Section - compact
    GtkWidget *additional_header = gtk_label_new("Additional Details");
    gtk_widget_set_halign(additional_header, GTK_ALIGN_START);
    gtk_widget_set_margin_top(additional_header, 3);
    gtk_widget_set_margin_bottom(additional_header, 2);
    gtk_grid_attach(GTK_GRID(grid), additional_header, 0, row++, 2, 1);
    
    // Other details - compact layout
    widgets->skills_entry = create_compact_labeled_entry(grid, "Skills:", row++);
    widgets->projects_entry = create_compact_labeled_entry(grid, "Projects:", row++);
    gtk_widget_set_size_request(widgets->projects_entry, 400, 40); // Reduced height
    
    widgets->achievements_entry = create_compact_labeled_entry(grid, "Achievements:", row++);
    gtk_widget_set_size_request(widgets->achievements_entry, 400, 40); // Reduced height
    
    widgets->languages_entry = create_compact_labeled_entry(grid, "Languages Known:", row++);
    widgets->hobbies_entry = create_compact_labeled_entry(grid, "Hobbies:", row++);
    
    // Create Generate button with compact layout
    button = gtk_button_new_with_label("Generate Resume");
    gtk_widget_set_margin_top(button, 5);
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    g_signal_connect(button, "clicked", G_CALLBACK(generate_resume), widgets);
    gtk_grid_attach(GTK_GRID(grid), button, 0, row, 2, 1);
    
    // Show all widgets
    gtk_widget_show_all(window);
}

// Helper function for creating compact labeled entries
GtkWidget* create_compact_labeled_entry(GtkWidget *grid, const char *label_text, int row) {
    GtkWidget *label = gtk_label_new(label_text);
    gtk_widget_set_halign(label, GTK_ALIGN_END);
    gtk_widget_set_margin_end(label, 2);  // Very small margin
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    
    GtkWidget *entry = gtk_entry_new();
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, row, 1, 1);
    
    return entry;
}

// Main function
int main(int argc, char *argv[]) {
    AppWidgets widgets;
    
    // Initialize GTK
    gtk_init(&argc, &argv);
    
    // Initialize widgets structure
    memset(&widgets, 0, sizeof(AppWidgets));
    
    // Start with authentication interface
    create_auth_interface(&widgets);
    
    // Start GTK main loop
    gtk_main();
    
  return 0;
}