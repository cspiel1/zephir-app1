#include <re_types.h>
#include <re_mod.h>

extern const struct mod_export exports_auconv;
extern const struct mod_export exports_auresamp;
extern const struct mod_export exports_auwm8960;
extern const struct mod_export exports_g711;
extern const struct mod_export exports_menu;
extern const struct mod_export exports_opus;


const struct mod_export *mod_table[] = {
  &exports_auconv,
  &exports_auresamp,
  &exports_auwm8960,
  &exports_g711,
  &exports_menu,
  &exports_opus,

  NULL
};
