/* Copyright 2012-present Facebook, Inc.
 * Licensed under the Apache License, Version 2.0 */
#pragma once
#include <thread>

enum trigger_input_style { input_dev_null, input_json, input_name_list };

struct watchman_trigger_command {
  w_string triggername;
  std::shared_ptr<w_query> query;
  json_ref definition;
  json_ref command;
  std::unordered_map<w_string, w_string> envht;

  bool append_files;
  enum trigger_input_style stdin_style;
  uint32_t max_files_stdin;

  int stdout_flags;
  int stderr_flags;
  const char *stdout_name;
  const char *stderr_name;

  /* While we are running, this holds the pid
   * of the running process */
  pid_t current_proc;

  watchman_trigger_command(
      const w_root_t* root,
      const json_ref& trig,
      char** errmsg);
  watchman_trigger_command(const watchman_trigger_command&) = delete;
  ~watchman_trigger_command();

  void stop();
  void start(const w_root_t* root);

 private:
  std::thread triggerThread_;
  std::shared_ptr<watchman::Publisher::Subscriber> subscriber_;
  w_evt_t ping_;
  bool stopTrigger_{false};

  void run(const w_root_t* root);
  bool maybeSpawn(unlocked_watchman_root* unlocked);
  bool waitNoIntr();
};

void w_assess_trigger(
    struct read_locked_watchman_root* lock,
    struct watchman_trigger_command* cmd);
std::unique_ptr<watchman_trigger_command> w_build_trigger_from_def(
    const w_root_t* root,
    const json_ref& trig,
    char** errmsg);
