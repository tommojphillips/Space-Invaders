/* ballbomb.h
 * Github: https:\\github.com\tommojphillips
 */

#ifdef __cplusplus
extern "C" {
#endif

int ballbomb_init();
void ballbomb_destroy();
void ballbomb_reset();
void ballbomb_update();
void ballbomb_vblank();
void ballbomb_save_state();
void ballbomb_load_state();

#ifdef __cplusplus
};
#endif