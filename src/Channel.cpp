/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 19:43:52 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/10 19:45:46 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Channel.hpp"

Channel::Channel( void ) {}

Channel::Channel( Channel const & copy ) {*this = copy;}

Channel::~Channel( void ) {}

Channel & Channel::operator=( Channel const & op ) {
	if (this != &op)
		return *this;
	return *this;
}
