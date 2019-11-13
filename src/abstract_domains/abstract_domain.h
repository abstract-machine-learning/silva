/**
 * Defines an abstract domain.
 *
 * @file abstract_domain.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef ABSTRACT_DOMAIN_H
#define ABSTRACT_DOMAIN_H

#include <stdio.h>


/** Types of abstract domains. */
typedef enum {
    DOMAIN_INTERVAL,       /**< Abstract domain of intervals: \f$\mathcal{Int}\f$. */
    DOMAIN_HYPERRECTANGLE  /**< Abstract domain of hyperrectangles: \f$\mathcal{Hyp}\f$. */
} AbstractDomainType;


/** Structure of an abstract domain. */
struct abstract_domain {
    AbstractDomainType type;  /**< Type of abstract domain. */
    void * data;              /**< Additional data of abstract domain. */
};


/** Type of an abstract domain. */
typedef struct abstract_domain AbstractDomain;



/**
 * Prints an abstract domain.
 *
 * @param[in] abstract_domain Abstract domain
 * @param[out] stream Stream
 */
void abstract_domain_print(
    const AbstractDomain abstract_domain,
    FILE *stream
);

#endif
