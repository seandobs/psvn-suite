#include <inttypes.h>

typedef int8_t var_t;

typedef struct {
 var_t vars[ NUMVARS ];
} state_t;

typedef struct {
  int size;
  var_t *v;
} abst_array_t;

typedef struct {
  var_t *value_map[ NUMDOMAINS ];
  uint8_t project_away_var[ NUMVARS ];
  abst_array_t* mapped_in[ NUMDOMAINS ];
  int* fwd_rule_label_sets;
  int* bwd_rule_label_sets;
} abstraction_t;

typedef struct {
  state_t state;
  int value;
} state_map_entry_t;

typedef struct {
  state_map_entry_t *entries;
  int64_t avail_entries;
  int64_t max_entry;
} state_map_t;
