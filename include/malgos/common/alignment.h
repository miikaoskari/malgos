#ifndef MLG_COMMON_ALIGNMENT
#define MLG_COMMON_ALIGNMENT

/* clang-format off */
#define mlg_is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#define mlg_align_down(x, a) ((x) & ~((a) - 1))
#define mlg_align_up(x, a) (((x) + (a) - 1) & ~((a) - 1))

/* clang-format on */

#endif /* MLG_COMMON_ALIGNMENT */
