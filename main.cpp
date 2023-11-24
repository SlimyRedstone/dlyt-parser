#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <windows.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

#define DL_FINAL_NAME \
  "-o \"%USERPROFILE%/Desktop/%(uploader|Unknown)s - %(title)s (%(webpage_url)s).%(ext)s\""
#define DL_FINAL_NAME_TO_DIR \
  "-o \"%USERPROFILE%/Desktop/%(uploader|YTDLP)s/%(uploader|Unknown)s - %(title)s.%(ext)s\""
#define CMD_START "start \"Youtube DL\" /D \"%USERPROFILE%\" /MIN"
#define SPONSOR_BLOCK_FILTERS                                               \
  string(                                                                   \
      "--sponsorblock-remove sponsor,selfpromo,interaction,music_offtopic " \
      "--sponsorblock-mark poi_highlight,chapter")

#define FILTER_Y \
  string("-f \"bestvideo[height<=2160]+bestaudio\" ") + string(DL_FINAL_NAME)
#define FILTER_X                                        \
  string("-f \"bestvideo[height<=2160]+bestaudio\" ") + \
      string(DL_FINAL_NAME_TO_DIR)
#define FILTER_M                                                               \
  string("-x --audio-quality 0 --audio-format wav --no-part --no-playlist ") + \
      string(DL_FINAL_NAME)
#define FILTER_V string("--no-playlist ") + string(DL_FINAL_NAME)

// #define DEBUG true
// #define RUN_AS_SEPARATE_PROCESS
string CURRENT_DIRECTORY = "";
bool download_status = false;

bool strReplaceAll(string &str, const string &from, const string &to) {
  if (from.empty()) return false;

  size_t start_i = 0;
  while ((start_i = str.find(from, start_i)) != string::npos) {
    str.replace(start_i, from.length(), to);
    start_i += to.length();
  }
  return true;
}

string filePath() {
  TCHAR buf[MAX_PATH] = {0};
  GetModuleFileName(NULL, buf, MAX_PATH);
  string::size_type pos = string(buf).find_last_of('\\');
  return string(buf).substr(0, pos);
}

const char *currentTime() {
  time_t t = time(0);
  return (const char *)ctime(&t);
}

string getRealURL(string url) {
  // strReplaceAll(url, "dlyt://", "https://");
  url = "https" + url.substr( url.find(':') );
  return url;
}

void logURL(string path, string url, char mode) {
  string file_path = path + "\\dlyt.log";
  FILE *log = fopen(file_path.c_str(), "a");
  url = getRealURL(url);
  fprintf(log, "%s→[%c] %s\n", currentTime(), mode, url.c_str());
  fclose(log);
}

void logText(string path, string text) {
  string file_path = path + "\\dlyt_raw.log";
  FILE *log = fopen(file_path.c_str(), "a");
  fprintf(log, text.c_str());
  fclose(log);
}

void dlVideo(string video_url, string filter) {
  video_url = getRealURL(video_url);
  string cmd = "%USERPROFILE%\\yt-dlp.exe";
  cmd = cmd + " -r 500M -R 3 --windows-filenames";
  cmd = cmd +
        " --progress --console-title --progress-template "
        "\"download-title:%(progress._percent_str)s -> %(info.uploader)s - "
        "%(info.title)s [%(info.id)s]\"";

  // cmd = cmd + " --exec after_move:\"echo •\"";
  cmd = cmd + " " + filter;
  cmd = cmd + " " + SPONSOR_BLOCK_FILTERS + " " + video_url;

  // printf("Command to be executed:\n%s", cmd.c_str());
  logText(CURRENT_DIRECTORY, cmd + "\n");

#ifdef RUN_AS_SEPARATE_PROCESS
  system((string(CMD_START) + " " + cmd).c_str());
#else
  system(cmd.c_str());
#endif
}

int main(int _n_args, char *_list_args[]) {
  CURRENT_DIRECTORY = filePath();

  uint16_t n_loop = 0;
  string download_url;
  char download_type = 'y';
  char options_list[2] = "";
  // char current_opt_value[256] = {};

  if (_n_args == 1) {
    printf(
        "No arguments\n"
        "-l [url] -> Video URL\n"
        "-m [mode] -> Can be y,m,v,x\n");
    printf("Current path: %s\n", CURRENT_DIRECTORY.c_str());
    return 0;
  } else if (_n_args >= 2) {
    const char *short_options = "l:m:";
    option long_options[] = {
        {"link", required_argument, NULL, 'l'},
        {"mode", optional_argument, NULL, 'm'},
        {nullptr, no_argument, nullptr, 0},
    };

    while (1) {
      const int _option = getopt_long(_n_args, _list_args, short_options,
                                      long_options, nullptr);


      if (_option == -1) {
        if (n_loop == 0) printf("Closing because no input arguments\n");
        break;
      }
      options_list[n_loop] = (char)_option;
      // strcpy(current_opt_value, optarg);
      // strncpy_s(current_opt_value,256,optarg,256);

      switch (_option) {
        case 'l': {
          // printf("Downloading url -> %s [%c]\n", current_opt_value, (const char)(_option));
          printf("Downloading url -> %s\n", optarg);
          // download_url = current_opt_value;
          download_url = optarg;
          break;
        }
        case 'm': {
          // printf("Downloading as -> %s [%c]\n", current_opt_value, (const char)(_option));
          printf("Downloading as -> %s\n", optarg);
          // download_type = current_opt_value[0];
          download_type = optarg[0];
          break;
        }
        default:
          break;
      }
      n_loop++;
      
    }
    if (strchr(options_list, 'm') == NULL) {
      printf(
          "Download link as: \n"
          " y -> High quality Youtube Video\n"
          " m -> Music\n"
          " v -> Video (Default)\n"
          " x -> Same as y, but extract to channel folder\n");
      // getline(cin, input);
      download_type = getchar();
    }
  }

  logURL(CURRENT_DIRECTORY, download_url, download_type);
  bool success_dl_file = false;

  do {
    switch (download_type) {
      case 'y': {
        printf("Downloading as High Quality Youtube video\n");
        dlVideo(download_url, FILTER_Y);
        success_dl_file = true;
        break;
      }
      case ' ':
      case '\n':
      case '\r':
      case 'v': {
        printf("Downloading as Video\n");
        dlVideo(download_url, FILTER_V);
        success_dl_file = true;
        break;
      }
      case 'm': {
        printf("Downloading as Music\n");
        dlVideo(download_url, FILTER_M);
        success_dl_file = true;
        break;
      }
      case 'x': {
        printf(
            "Downloading as High Quality Youtube video, then extract to "
            "channel's folder\n");
        dlVideo(download_url, FILTER_X);
        success_dl_file = true;
        break;
      }

      case '\0':
      default: {
        // success_dl_file = true;
        printf("Unknown command, using 'y' mode as default\n");
        dlVideo(download_url, FILTER_Y);
        break;
      }
    }
  } while (!success_dl_file);

  /* while(!download_status) {
    download_status = system("pause") == 0;
  } */

  return 0;
}