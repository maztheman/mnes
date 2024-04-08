#pragma once

namespace mnes::common {
  bool is_cpu_running();
  bool is_display_ready();
  bool is_pattern_table_ready();
  bool is_name_table_ready();

  void set_cpu_running(bool value);
  void set_display_ready(bool value);
  void set_pattern_table_ready(bool value);
  void set_name_table_ready(bool value);
  void set_all_display_ready(bool value);
}
