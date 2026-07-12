ENTRY(_start)

/* ELF のエントリポイントは boot.asm の _start。 */

PHDRS
{
    /* FLAGS(5) = R + X。コードと読み取り専用データ用。 */
    text PT_LOAD FLAGS(5);

    /* FLAGS(6) = R + W。書き換えるデータと BSS 用。 */
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    /* x86 の Multiboot カーネルを 1MiB から配置する。 */
    . = 1M;

    .text :
    {
        /* Multiboot header は先頭付近に必要なので .text の最初へ置く。 */
        *(.multiboot)
        *(.text*)
    } :text

    .rodata :
    {
        /* 文字列リテラルなどの読み取り専用データ。 */
        *(.rodata*)
    } :text

    /* 書き換える領域はページ境界にそろえて別 LOAD segment にする。 */
    . = ALIGN(4K);

    .data :
    {
        /* 初期値ありのグローバル/static 変数。 */
        *(.data*)
    } :data

    .bss :
    {
        /* 初期値なし、または 0 初期化されるグローバル/static 変数。 */
        *(COMMON)
        *(.bss*)
    } :data
}
