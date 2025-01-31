/* invaderspt2.h
 * Github: https:\\github.com\tommojphillips
 */

#ifdef __cplusplus
extern "C" {
#endif

int invaderspt2_init();
void invaderspt2_destroy();
void invaderspt2_reset();
void invaderspt2_update();
void invaderspt2_vblank();
void invaderspt2_save_state();
void invaderspt2_load_state();

#ifdef __cplusplus
};
#endif
