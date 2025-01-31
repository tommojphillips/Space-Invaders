/* lrescue.h
 * Github: https:\\github.com\tommojphillips
 */

#ifdef __cplusplus
extern "C" {
#endif

int lrescue_init();
void lrescue_destroy();
void lrescue_reset();
void lrescue_update();
void lrescue_vblank();
void lrescue_save_state();
void lrescue_load_state();

#ifdef __cplusplus
};
#endif
