export const idlFactory = ({ IDL }) => {
  return IDL.Service({
    'play_match' : IDL.Func([IDL.Nat64], [IDL.Text], ['query']),
  });
};
export const init = ({ IDL }) => { return []; };
