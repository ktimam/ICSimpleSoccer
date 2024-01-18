import type { Principal } from '@dfinity/principal';
import type { ActorMethod } from '@dfinity/agent';

export type Result = { 'ok' : string } |
  { 'err' : StatusCode };
export type StatusCode = number;
export interface _SERVICE { 'play_match' : ActorMethod<[bigint], string> }
