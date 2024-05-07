import type { Principal } from '@dfinity/principal';
import type { ActorMethod } from '@dfinity/agent';
import type { IDL } from '@dfinity/candid';

export type Result = { 'ok' : string } |
  { 'err' : StatusCode };
export type StatusCode = number;
export interface _SERVICE { 'play_match' : ActorMethod<[bigint], string> }
export declare const idlFactory: IDL.InterfaceFactory;
export declare const init: (args: { IDL: typeof IDL }) => IDL.Type[];
