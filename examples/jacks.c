/* 
 * jacks.c
 * Copyright 2013 Chris Moore
 * 
 * An example program for the poker hand evaluation library.
 * 
 * Given a list of 5 cards and a jackpot amount, this program
 * determines the best cards to hold for jacks or better video poker.
 *
 * This program gives you software freedom; you can copy, convey,
 * propagate, redistribute and/or modify this program under the terms of
 * the GNU General Public License (GPL) as published by the Free Software
 * Foundation (FSF), either version 3 of the License, or (at your option)
 * any later version of the GPL published by the FSF.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program in a file in the toplevel directory called "GPLv3".
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poker_defs.h"
#include "inlines/eval_type.h"

float jackpot_payout[StdRules_HandType_LAST+1] = {0, 5, 10, 15, 20, 30, 45, 125, 250, 4000};
float regular_payout[StdRules_HandType_LAST+1] = {0, 5, 10, 15, 20, 30, 45, 125, 250, 4000};

int 
main(int argc, char **argv) {
  CardMask gDeadCards, gPeggedCards;

  CardMask_RESET(gDeadCards);
  CardMask_RESET(gPeggedCards);

  int c1, c2, c3, c4, c5;
  float pot;

  if (argc != 7 ||
      Deck_stringToCard(argv[2], &c1) == 0 ||
      Deck_stringToCard(argv[3], &c2) == 0 ||
      Deck_stringToCard(argv[4], &c3) == 0 ||
      Deck_stringToCard(argv[5], &c4) == 0 ||
      Deck_stringToCard(argv[6], &c5) == 0)
    goto error;

  sscanf(argv[1], "%g", &pot);
  jackpot_payout[StdRules_HandType_LAST] = pot;

  printf("cards: %s %s %s %s %s and pot %f\n", argv[2], argv[3], argv[4], argv[5], argv[6], pot);

  CardMask_SET(gDeadCards, c1);
  CardMask_SET(gDeadCards, c2);
  CardMask_SET(gDeadCards, c3);
  CardMask_SET(gDeadCards, c4);
  CardMask_SET(gDeadCards, c5);

  int h1, h2, h3, h4, h5;
  int j1, j2, j3, j4, j5;
  int r1, r2, r3, r4, r5;
  float jackpot_best = 0, regular_best = 0;
  char jackpot_cards[20], regular_cards[20];
  int gNPegged = 0;
  uint32 totals[HandType_LAST+1];
  int i;

  CardMask_SET(gPeggedCards, c1); ++gNPegged;
  CardMask_SET(gPeggedCards, c2); ++gNPegged;
  CardMask_SET(gPeggedCards, c3); ++gNPegged;
  CardMask_SET(gPeggedCards, c4); ++gNPegged;
  CardMask_SET(gPeggedCards, c5); ++gNPegged;

  for (h1 = 0; h1 < 2; h1++) {
    if (h1) { CardMask_SET(gPeggedCards, c1); ++gNPegged; } else { CardMask_UNSET(gPeggedCards, c1); --gNPegged; }
    for (h2 = 0; h2 < 2; h2++) {
      if (h2) { CardMask_SET(gPeggedCards, c2); ++gNPegged; } else { CardMask_UNSET(gPeggedCards, c2); --gNPegged; }
      for (h3 = 0; h3 < 2; h3++) {
	if (h3) { CardMask_SET(gPeggedCards, c3); ++gNPegged; } else { CardMask_UNSET(gPeggedCards, c3); --gNPegged; }
	for (h4 = 0; h4 < 2; h4++) {
	  if (h4) { CardMask_SET(gPeggedCards, c4); ++gNPegged; } else { CardMask_UNSET(gPeggedCards, c4); --gNPegged; }
	  for (h5 = 0; h5 < 2; h5++) {
	    if (h5) { CardMask_SET(gPeggedCards, c5); ++gNPegged; } else { CardMask_UNSET(gPeggedCards, c5); --gNPegged; }

	    for (i = 0; i < HandType_LAST+1; i++) totals[i] = 0;
	    int handtype, nHands=0;
	    CardMask hand, cards;

	    /* We use the fast and small EVAL_TYPE evaluator, since we only care 
	       about the hand type, not the particular cards.  If we cared about the
	       individual cards, we'd use EVAL_N. */
	    ENUMERATE_N_CARDS_D(cards, (5-gNPegged), gDeadCards,
				{
				  CardMask_OR(hand, cards, gPeggedCards);
				  handtype = Hand_EVAL_TYPE(hand, 5);
				  ++nHands;
				  ++totals[handtype];
				});

	    float jackpot_pay = 0, regular_pay = 0;
	    for (i = HandType_FIRST; i <= HandType_LAST; i++) {
	      jackpot_pay += jackpot_payout[i] * totals[i];
	      regular_pay += regular_payout[i] * totals[i];
	    }

	    float jackpot_score = jackpot_pay/nHands;
	    float regular_score = regular_pay/nHands;

	    if (jackpot_score > jackpot_best) {
	      jackpot_best = jackpot_score;
	      strcpy(jackpot_cards, Deck_maskString(gPeggedCards));
	      j1 = h1; j2 = h2; j3 = h3; j4 = h4; j5 = h5;
	    }

	    if (regular_score > regular_best) {
	      regular_best = regular_score;
	      strcpy(regular_cards, Deck_maskString(gPeggedCards));
	      r1 = h1; r2 = h2; r3 = h3; r4 = h4; r5 = h5;
	    }
	  }
	}
      }
    }
  }

  if (r1 != j1 || r2 != j2 || r3 != j3 || r4 != j4 || r5 != j5)
    printf("%d%d%d%d%d (%s) is standard, but jackpot %f means we should play differently:\n", r1, r2, r3, r4, r5, regular_cards, pot);
  else
    printf("standard play is optimal for jackpot %f\n", pot);

  printf("%d%d%d%d%d (%s)\n", j1, j2, j3, j4, j5, jackpot_cards);

  exit(0);

 error:
  fprintf(stderr, "Usage: jacks jackpot card1 card2 card3 card4 card5\n");
  exit(1);
}
