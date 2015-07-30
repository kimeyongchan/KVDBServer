/**
 * @file    aop_x86_64.h
 * @brief   TODO brief documentation here.
 *
 * @history
 *  - 파일 생성 및 초기 구현 (2013.01 백설기)
 */

#ifndef _AOP_X86_64_H
#define _AOP_X86_64_H

#define LOCK "lock ; "

typedef long aop_val_t;
typedef aop_val_t aop_lock_t;

#define aop_lock_init(l)    aop_store(l, 0)

/**
 * @brief   vp에 담긴 값을 읽는다.
 *
 * [semantics]
 *      atomic {
 *          rv = *vp;
 *      }
 *      return rv;
 *
 * @param[in]  vp: 읽을 값의 주소
 *
 * @return  읽은 값
 */
static inline long
aop_load(volatile aop_val_t *vp)
{
    return *vp;
}

/**
 * @brief   vp에 nv 값을 쓴다.
 *
 * [semantics]
 *      atomic {
 *          *vp = nv;
 *      }
 *
 * @param[out]  vp: nv를 쓸 위치
 * @param[in]   nv: vp에 쓸 값
 */
static inline void
aop_store(volatile aop_val_t *vp, long nv)
{
    *vp = nv;
}

/**
 * @brief   *vp에 v를 더해서 vp에 쓴다.
 *
 * [semantics]
 *      atomic {
 *          *vp += v;
 *      }
 *
 * @param[in,out]   vp: 더하고 쓸 위치
 * @param[in]       v : 더할 값
 */
static inline void
aop_add(volatile aop_val_t *vp, long v)
{
    asm __volatile__(
        LOCK "addq %1,%0"
        :"=m" (*vp)
        :"ir" (v), "m" (*vp)
    );
}

/**
 * @brief   *vp에서 v를 뺀 후 vp에 쓴다
 *
 * [semantics]
 *      atomic {
 *          *vp -= v;
 *      }
 *
 * @param[in,out]   vp: 빼고 쓸 위치
 * @param[in]       v : 뺄 값
 */
static inline void
aop_sub(volatile aop_val_t *vp, long v)
{
    asm __volatile__(
        LOCK "subq %1,%0"
        :"=m" (*vp)
        :"ir" (v), "m" (*vp)
    );
}

/**
 * @brief   *vp에서 1를 증가시킨 후 vp에 쓴다
 *
 * [semantics]
 *      atomic {
 *          *vp += 1;
 *      }
 *
 * @param[in,out]   vp: 1 증가후 쓸 위치
 */
static inline void
aop_inc(volatile aop_val_t *vp)
{
    asm __volatile__(
        LOCK "incq %0"
        :"=m" (*vp)
        :"m" (*vp)
    );
}

/**
 * @brief   *vp에서 1를 감소시킨 후 vp에 쓴다
 *
 * [semantics]
 *      atomic {
 *          *vp -= 1;
 *      }
 *
 * @param[in,out]   vp: 1 감소후 쓸 위치
 */
static inline void
aop_dec(volatile aop_val_t *vp)
{
    asm __volatile__(
        LOCK "decq %0"
        :"=m" (*vp)
        :"m" (*vp)
    );
}

/**
 * @brief   *vp에 v를 더해서 vp에 쓰고 쓰여진 값을 리턴한다
 *
 * [semantics]
 *      atomic {
 *          ov = *vp;
 *          *vp += v;
 *      }
 *      return ov + v;
 *
 * @param[in,out]   vp: 더하고 쓸 위치
 * @param[in]       v : 더할 값
 *
 * @return  vp에 쓰여진 값(new value)
 *
 * @warning xaddl이 지원되는 486+ 이상만 가능
 */
static inline long
aop_add_nv(volatile aop_val_t *vp, long v)
{
    long tv;

    tv = v;
    asm __volatile__(
        LOCK "xaddq %0, %1"
        :"+r" (v), "+m" (*vp)
        : : "memory"
    );

    return v + tv;
}

/**
 * @brief   *vp에 v를 뺀 후 vp에 쓰고 쓰여진 값을 리턴한다
 *
 * [semantics]
 *      atomic {
 *          ov = *vp;
 *          *vp -= v;
 *      }
 *      return ov - v;
 *
 * @param[in,out]   vp: 빼고 쓸 위치
 * @param[in]       v : 뺄 값
 *
 * @return  vp에 쓰여진 값(new value)
 */
#define aop_sub_nv(vp, v) aop_add_nv(vp, -(v))

/**
 * @brief   *vp에서 1를 증가시킨 후 vp에 쓰고, 쓰여진 값을 리턴한다
 *
 * [semantics]
 *      atomic {
 *          ov = *vp;
 *          *vp += 1;
 *      }
 *      return ov + 1;
 *
 * @param[in,out]   vp: 1 증가 후 쓸 위치
 *
 * @return  vp에 쓰여진 값(new value)
 */
#define aop_inc_nv(vp) aop_add_nv(vp, 1)

/**
 * @brief   *vp에서 1를 감소시킨 후 vp에 쓰고, 쓰여진 값을 리턴한다
 *
 * [semantics]
 *      atomic {
 *          ov = *vp;
 *          *vp -= 1;
 *      }
 *      return ov - 1;
 *
 * @param[in,out]   vp: 1 감소 후 쓸 위치
 *
 * @return  vp에 쓰여진 값(new value)
 */
#define aop_dec_nv(vp) aop_add_nv(vp, -1)

/**
 * @brief   *vp를 v와 AND(bits test)를 한다.
 *
 * [semantics]
 *      atomic {
 *          rv = *vp & v;
 *      }
 *      return rv;
*
 * @param[in]   vp: test할 값의 위치
 * @param[in]   v : test할 bit mask
 *
 * @return  test의 결과
 */
static inline long
aop_test(volatile aop_val_t *vp, long v)
{
    return aop_load(vp) & v;
}

/**
 * @brief   *vp와 v를 OR 한 후 vp에 쓴다
 *
 * [semantics]
 *      atomic {
 *          *vp |= v;
 *      }
 *
 * @param[in,out]   vp: OR 하고 쓸 위치
 * @param[in]       v : OR 할 mask
 */
static inline void
aop_or(volatile aop_val_t *vp, long v)
{
    asm __volatile__(
        LOCK "orq %1,%0"
        :"+m" (*vp)
        :"ir" (v)
    );
}

/**
 * @brief   *vp와 v를 AND 한 후 vp에 쓴다
 *
 * [semantics]
 *      atomic {
 *          *vp &= v;
 *      }
 *
 * @param[in,out]   vp: AND 하고 쓸 위치
 * @param[in]       v : OR 할 mask
 */
static inline void
aop_and(volatile aop_val_t *vp, long v)
{
    asm __volatile__(
        LOCK "andq %1,%0"
        :"+m" (*vp)
        :"ir" (v)
    );
}

/**
 * @brief   *vp와 ov를 비교한 후 같으면 nv를 vp에 쓰고 old value를 리턴한다
 *
 * [semantics]
 *      atomic {
 *          pv = *vp;
 *          if (*vp == ov)
 *              *vp = nv;
 *      }
 *      return pv;
 *
 * @param[in,out]   vp
 * @param[in]       nv
 * @param[in]       ov
 *
 * @return  nv를 쓰기 전의 vp의 old value
 */
static inline long
aop_cas(volatile aop_val_t *vp, long nv, long ov)
{
    asm __volatile__(
        LOCK "cmpxchgq %1,(%3)"
        : "=a" (nv)
        : "r" (nv), "a" (ov), "r" (vp)
        : "cc", "memory"
    );

    return nv;
}

/**
 * @brief   *vp와 v를 swap한 후에 v를 리턴한다
 *          cas와는 달리 무조건 vp에 새로운 값이 쓰여지게 된다
 *
 * [semantics]
 *      atomic {
 *          ov = *vp;
 *          *vp = v;
 *      }
 *      return ov;
 *
 * @param[in]   vp
 * @param[in]   v
 *
 * @return  vp의 old value가 리턴된다
 */
static inline long
aop_swp(volatile aop_val_t *vp, long v)
{
    asm __volatile__(
        "xchgq %0,%1"
        :"=r" (v)
        :"m" (*vp), "0" (v)
        :"memory"
    );

    return v;
}

/**
 * @brief   *vp와 v를 OR 한 후 vp에 다시 쓰고 vp의 old value를 리턴한다
 *
 * [semantics]
 *      atomic {
 *          ov = *vp;
 *          *vp |= v;
 *      }
 *      return ov;
 *
 * @param[in,out]   vp: OR 하고 쓸 위치
 * @param[in]       v : OR 할 mask
 *
 * @return  OR하기 전의 vp의 old value
 */
static inline long
aop_or_ov(volatile aop_val_t *vp, long v)
{
    long ov, nv;

    do {
        ov = *vp;
        nv = ov|v;
    } while (aop_cas(vp, nv, ov) != ov);

    return ov;
}

/**
 * @brief   *vp와 v를 AND 한 후 vp에 다시 쓰고 vp의 old value를 리턴한다
 *
 * [semantics]
 *      atomic {
 *          ov = *vp;
 *          *vp &= v;
 *      }
 *      return ov;
 *
 * @param[in,out]   vp: AND 하고 쓸 위치
 * @param[in]       v : AND 할 mask
 *
 * @return  OR하기 전의 vp의 old value
 */
static inline long
aop_and_ov(volatile aop_val_t *vp, long v)
{
    long ov, nv;

    do {
        ov = *vp;
        nv = ov&v;
    } while (aop_cas(vp, nv, ov) != ov);

    return ov;
}

/**
 * @brief  read/write memory barrier(compiler and processor)
 *
 * barrier 이전에 수행된 read/write memory operation은 이 barrier 이전에 모두
 * 완료된다.
 */

#define aop_mb()    asm volatile("mfence":::"memory")

/**
 * @brief   read memory barrier(compiler and processor)
 *
 * barrier 이전에 수행된 read memory operation은 이 barrier 이전에 모두
 * 완료된다.
 */
#define aop_rmb()   asm volatile("lfence":::"memory")

/**
 * @brief   write memory barrier(compiler and processor)
 *
 * barrier 이전에 수행된 write memory operation은 이 barrier 이전에 모두
 * 완료된다.
 */
#define aop_wmb()   asm volatile("sfence" ::: "memory")

static inline void
aop_lock(volatile aop_lock_t *lk)
{
    while (aop_cas(lk, 1, 0))
        ;
    aop_mb();
}

static inline void
aop_unlock(volatile aop_lock_t *lk)
{
    aop_mb();
    aop_store(lk, 0);
}

static inline bool
aop_try_lock(volatile aop_lock_t *lk)
{
    if (aop_cas(lk, 1, 0))
        return false;

    aop_mb();
    return true;
}

#endif /* no _AOP_X86_64_H */
