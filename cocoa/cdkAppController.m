/*********************************************************
 * Copyright (C) 2009 VMware, Inc. All rights reserved.
 *
 * This file is part of VMware View Open Client.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation version 2.1 and no later version.
 *
 * This program is released with an additional exemption that
 * compiling, linking, and/or using the OpenSSL libraries with this
 * program is allowed.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the Lesser GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.
 *
 *********************************************************/

/*
 * cdkAppController.m --
 *
 *     Implementation of CdkAppController
 */

#import "cdkAppController.h"
#import "cdkWindowController.h"


@interface CdkAppController () // Private setters
@property(readwrite, retain) CdkWindowController *windowController;
@end // @interface CdkAppController ()


@implementation CdkAppController


@synthesize windowController;


/*
 *-----------------------------------------------------------------------------
 *
 * -[CdkAppController dealloc] --
 *
 *      Release our resources.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Releases views.
 *
 *-----------------------------------------------------------------------------
 */

-(void)dealloc
{
   [self setWindowController:nil];
   [super dealloc];
}


/*
 *-----------------------------------------------------------------------------
 *
 * -[CdkAppController awakeFromNib] --
 *
 *      Awake handler.  Prompt the user for a server.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Broker screen is displayed.
 *
 *-----------------------------------------------------------------------------
 */

-(void)awakeFromNib
{
   [self setWindowController:[CdkWindowController windowController]];
   [windowController showWindow:self];
}


@end // @implementation CdkAppController
