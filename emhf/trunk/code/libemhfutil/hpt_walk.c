/*
 * @XMHF_LICENSE_HEADER_START@
 *
 * eXtensible, Modular Hypervisor Framework (XMHF)
 * Copyright (c) 2009-2012 Carnegie Mellon University
 * Copyright (c) 2010-2012 VDG Inc.
 * All Rights Reserved.
 *
 * Developed by: XMHF Team
 *               Carnegie Mellon University / CyLab
 *               VDG Inc.
 *               http://xmhf.org
 *
 * This file is part of the EMHF historical reference
 * codebase, and is released under the terms of the
 * GNU General Public License (GPL) version 2.
 * Please see the LICENSE file for details.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @XMHF_LICENSE_HEADER_END@
 */

#include <hpt.h>

/* attempt to descend one level. on success, lvl and pm are set
   accordingly, and true is returned. on failure, lvl and pm are
   untouched and false is returned. */
bool hpt_walk_next_lvl(const hpt_walk_ctx_t *ctx, int *lvl, hpt_pm_t *pm, hpt_va_t va)
{
  hpt_pme_t pme = hpt_pm_get_pme_by_va(ctx->t, *lvl, *pm, va);
  if (!hpt_pme_is_present(ctx->t, *lvl, pme)
      || hpt_pme_is_page(ctx->t, *lvl, pme)) {
    return false;
  } else {
    *pm = ctx->pa2ptr(ctx->pa2ptr_ctx, hpt_pme_get_address(ctx->t, *lvl, pme));
    (*lvl)--;
    return true;
  }
}

/* returns the lowest-level page map containing va, down to
 * end_lvl. end_lvl is set to the level of the returned page map.
 */
hpt_pm_t hpt_walk_get_pm(const hpt_walk_ctx_t *ctx, int lvl, hpt_pm_t pm, int *end_lvl, hpt_va_t va)
{
  assert(lvl >= *end_lvl);

  while(lvl > *end_lvl) {
    if (!hpt_walk_next_lvl(ctx, &lvl, &pm, va)) {
      *end_lvl = lvl;
      return pm;
    }
  }
  return pm;
}

/* returns the lowest-level page map _entry_ containing va, down to
 * end_lvl. end_lvl is set to the level of the returned page map
 * containing the returned entry.
 */
hpt_pme_t hpt_walk_get_pme(const hpt_walk_ctx_t *ctx, int lvl, hpt_pm_t pm, int *end_lvl, hpt_va_t va)
{
  pm = hpt_walk_get_pm(ctx, lvl, pm, end_lvl, va);
  return hpt_pm_get_pme_by_va(ctx->t, *end_lvl, pm, va);
}

/* returns the page map of level end_lvl containing va, allocating
   maps if necessary. Note that the end_lvl may be a higher level than requested
   if the address is mapped via a large page.
*/
hpt_pm_t hpt_walk_get_pm_alloc(const hpt_walk_ctx_t *ctx, int lvl, hpt_pm_t pm, int *end_lvl, hpt_va_t va)
{
  assert(lvl >= *end_lvl);
  while(lvl > *end_lvl) {
    hpt_pme_t pme = hpt_pm_get_pme_by_va(ctx->t, lvl, pm, va);

    hpt_log_trace("hpt_walk_get_pm_alloc: lvl:%d pm:%x end_lvl:%d va:%Lx\n",
                  lvl, (u32)pm, *end_lvl, va);
    hpt_log_trace("hpt_walk_get_pm_alloc: pme:%Lx\n",
            pme);
    if (hpt_pme_is_page(ctx->t, lvl, pme)) {
      *end_lvl = lvl;
      return pm;
    }
    if (!hpt_pme_is_present(ctx->t, lvl, pme)) {
      hpt_pm_t new_pm = ctx->gzp(ctx->gzp_ctx,
                                 HPT_PM_SIZE/*FIXME*/,
                                 hpt_pm_size(ctx->t, lvl-1));
      hpt_log_trace("hpt_walk_get_pm_alloc: allocated pm at hva:%x spa:%Lx\n",
              (u32)new_pm, ctx->ptr2pa(ctx->ptr2pa_ctx, new_pm));
      if(!new_pm) {
        return NULL;
      }
      pme = hpt_pme_set_address(ctx->t, lvl, pme, ctx->ptr2pa(ctx->ptr2pa_ctx, new_pm));
      pme = hpt_pme_setprot(ctx->t, lvl, pme, HPT_PROTS_RWX);
      pme = hpt_pme_setuser(ctx->t, lvl, pme, true);
      hpt_pm_set_pme_by_va(ctx->t, lvl, pm, va, pme);
      hpt_log_trace("hpt_walk_get_pm_alloc: inserted pme:%Lx\n", pme);
    }
    assert(hpt_walk_next_lvl(ctx, &lvl, &pm, va));
  }
  assert(lvl==*end_lvl);
  return pm;
}

/* inserts pme into the page map of level tgt_lvl containing va, allocating
 * maps if necessary. returns 0 on success, other on failure.
 * Will fail if one of the intermediate entries is a large page
 */
int hpt_walk_insert_pme_alloc(const hpt_walk_ctx_t *ctx, int lvl, hpt_pm_t pm, int tgt_lvl, hpt_va_t va, hpt_pme_t pme)
{
  int end_lvl=tgt_lvl;
  hpt_log_trace("hpt_walk_insert_pme_alloc: lvl:%d pm:%x tgt_lvl:%d va:%Lx pme:%Lx\n",
          lvl, (u32)pm, tgt_lvl, va, pme);
  pm = hpt_walk_get_pm_alloc(ctx, lvl, pm, &end_lvl, va);
  hpt_log_trace("hpt_walk_insert_pme_alloc: got pm:%x end_lvl:%d\n",
          (u32)pm, end_lvl);

  if(pm == NULL || tgt_lvl != end_lvl) {
    return 1;
  }
  hpt_pm_set_pme_by_va(ctx->t, tgt_lvl, pm, va, pme);
  return 0;
}

void hpt_walk_set_prot(hpt_walk_ctx_t *walk_ctx, hpt_pm_t pm, int pm_lvl, hpt_va_t va, hpt_prot_t prot)
{
  hpt_pme_t pme;
  int end_pm_lvl=1;

  pm = hpt_walk_get_pm(walk_ctx, pm_lvl, pm, &end_pm_lvl, va);
  assert(pm != NULL);
  assert(end_pm_lvl==1); /* FIXME we don't handle large pages */
  pme = hpt_pm_get_pme_by_va(walk_ctx->t, end_pm_lvl, pm, va);
  pme = hpt_pme_setprot(walk_ctx->t, end_pm_lvl, pme, prot);
  hpt_pm_set_pme_by_va(walk_ctx->t, end_pm_lvl, pm, va, pme);
}

void hpt_walk_set_prots(hpt_walk_ctx_t *walk_ctx,
                        hpt_pm_t pm,
                        int pm_lvl,
                        hpt_va_t vas[],
                        size_t num_vas,
                        hpt_prot_t prot)
{
  size_t i;
  for(i=0; i<num_vas; i++) {
    hpt_walk_set_prot(walk_ctx, pm, pm_lvl, vas[i], prot);
  }
}

/* inserts pme into the page map of level tgt_lvl containing va.
 * fails if tgt_lvl is not allocated.
 */
int hpt_walk_insert_pme(const hpt_walk_ctx_t *ctx, int lvl, hpt_pm_t pm, int tgt_lvl, hpt_va_t va, hpt_pme_t pme)
{
  int end_lvl=tgt_lvl;
  hpt_log_trace("hpt_walk_insert_pme_alloc: lvl:%d pm:%x tgt_lvl:%d va:%Lx pme:%Lx\n",
                lvl, (u32)pm, tgt_lvl, va, pme);
  pm = hpt_walk_get_pm(ctx, lvl, pm, &end_lvl, va);

  hpt_log_trace("hpt_walk_insert_pme: got pm:%x end_lvl:%d\n",
                (u32)pm, end_lvl);

  if(pm == NULL || tgt_lvl != end_lvl) {
    return 1;
  }

  hpt_pm_set_pme_by_va(ctx->t, tgt_lvl, pm, va, pme);
  return 0;
}
