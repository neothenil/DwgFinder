"""
Copyright 2010 Pallets

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or other
materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may
be used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
"""
from __future__ import annotations

import typing as t
from math import ceil

from flask import abort
from flask import request


class Pagination:
    """Modified from ``Pagination`` class in flask_sqlalchemy.

    Apply an offset and limit to the query based on the current page and number of
    items per page.

    :param page: The current page, used to calculate the offset. Defaults to the
        ``page`` query arg during a request, or 1 otherwise.
    :param per_page: The maximum number of items on a page, used to calculate the
        offset and limit. Defaults to the ``per_page`` query arg during a request,
        or 20 otherwise.
    :param max_per_page: The maximum allowed value for ``per_page``, to limit a
        user-provided value. Use ``None`` for no limit. Defaults to 100.
    :param error_out: Abort with a ``404 Not Found`` error if no items are returned
        and ``page`` is not 1, or if ``page`` or ``per_page`` is less than 1, or if
        either are not ints.
    :param count: Calculate the total number of values by issuing an extra count
        query. For very complex queries this may be inaccurate or slow, so it can be
        disabled and set manually if necessary.
    :param kwargs: Information about the query to paginate. Different subclasses will
        require different arguments.
    """

    def __init__(
        self,
        page: int | None = None,
        per_page: int | None = None,
        max_per_page: int | None = 100,
        error_out: bool = True,
        count: bool = True,
        **kwargs: t.Any,
    ) -> None:
        self._query_args = kwargs
        page, per_page = self._prepare_page_args(
            page=page,
            per_page=per_page,
            max_per_page=max_per_page,
            error_out=error_out,
        )

        self.page: int = page
        """The current page."""

        self.per_page: int = per_page
        """The maximum number of items on a page."""

        items = self._query_items()

        if not items and page != 1 and error_out:
            abort(404)

        self.items: list[t.Any] = items
        """The items on the current page. Iterating over the pagination object is
        equivalent to iterating over the items.
        """

        if count:
            total = self._query_count()
        else:
            total = None

        self.total: int | None = total
        """The total number of items across all pages."""

    @staticmethod
    def _prepare_page_args(
        *,
        page: int | None = None,
        per_page: int | None = None,
        max_per_page: int | None = None,
        error_out: bool = True,
    ) -> tuple[int, int]:
        if request:
            if page is None:
                try:
                    page = int(request.args.get("page", 1))
                except (TypeError, ValueError):
                    if error_out:
                        abort(404)

                    page = 1

            if per_page is None:
                try:
                    per_page = int(request.args.get("per_page", 20))
                except (TypeError, ValueError):
                    if error_out:
                        abort(404)

                    per_page = 20
        else:
            if page is None:
                page = 1

            if per_page is None:
                per_page = 20

        if max_per_page is not None:
            per_page = min(per_page, max_per_page)

        if page < 1:
            if error_out:
                abort(404)
            else:
                page = 1

        if per_page < 1:
            if error_out:
                abort(404)
            else:
                per_page = 20

        return page, per_page

    @property
    def _query_offset(self) -> int:
        """The index of the first item to query, passed to ``offset()``."""
        return (self.page - 1) * self.per_page

    def _query_items(self) -> list[t.Any]:
        """Execute the query to get the items on the current page.

        Uses init arguments stored in :attr:`_query_args`.
        """
        db = self._query_args["db"]
        sql = self._query_args["sql"]
        select = f"{sql} limit {self.per_page} offset {self._query_offset}"
        return db.execute(select).fetchall()

    def _query_count(self) -> int:
        """Execute the query to get the total number of items.

        Uses init arguments stored in :attr:`_query_args`.
        """
        db = self._query_args["db"]
        sql = self._query_args["sql"]
        select = sql.replace("*", "count(*)")
        return db.execute(select).fetchone()["count(*)"]

    @property
    def first(self) -> int:
        """The number of the first item on the page, starting from 1, or 0 if there are
        no items.
        """
        if len(self.items) == 0:
            return 0

        return (self.page - 1) * self.per_page + 1

    @property
    def last(self) -> int:
        """The number of the last item on the page, starting from 1, inclusive, or 0 if
        there are no items.
        """
        first = self.first
        return max(first, first + len(self.items) - 1)

    @property
    def pages(self) -> int:
        """The total number of pages."""
        if self.total == 0 or self.total is None:
            return 0

        return ceil(self.total / self.per_page)

    @property
    def has_prev(self) -> bool:
        """``True`` if this is not the first page."""
        return self.page > 1

    @property
    def prev_num(self) -> int | None:
        """The previous page number, or ``None`` if this is the first page."""
        if not self.has_prev:
            return None

        return self.page - 1

    def prev(self, *, error_out: bool = False) -> Pagination:
        """Query the :class:`Pagination` object for the previous page.

        :param error_out: Abort with a ``404 Not Found`` error if no items are returned
            and ``page`` is not 1, or if ``page`` or ``per_page`` is less than 1, or if
            either are not ints.
        """
        p = type(self)(
            page=self.page - 1,
            per_page=self.per_page,
            error_out=error_out,
            count=False,
            **self._query_args,
        )
        p.total = self.total
        return p

    @property
    def has_next(self) -> bool:
        """``True`` if this is not the last page."""
        return self.page < self.pages

    @property
    def next_num(self) -> int | None:
        """The next page number, or ``None`` if this is the last page."""
        if not self.has_next:
            return None

        return self.page + 1

    def next(self, *, error_out: bool = False) -> Pagination:
        """Query the :class:`Pagination` object for the next page.

        :param error_out: Abort with a ``404 Not Found`` error if no items are returned
            and ``page`` is not 1, or if ``page`` or ``per_page`` is less than 1, or if
            either are not ints.
        """
        p = type(self)(
            page=self.page + 1,
            per_page=self.per_page,
            error_out=error_out,
            count=False,
            **self._query_args,
        )
        p.total = self.total
        return p

    def iter_pages(
        self,
        *,
        left_edge: int = 2,
        left_current: int = 2,
        right_current: int = 4,
        right_edge: int = 2,
    ) -> t.Iterator[int | None]:
        """Yield page numbers for a pagination widget. Skipped pages between the edges
        and middle are represented by a ``None``.

        For example, if there are 20 pages and the current page is 7, the following
        values are yielded.

        .. code-block:: python

            1, 2, None, 5, 6, 7, 8, 9, 10, 11, None, 19, 20

        :param left_edge: How many pages to show from the first page.
        :param left_current: How many pages to show left of the current page.
        :param right_current: How many pages to show right of the current page.
        :param right_edge: How many pages to show from the last page.
        """
        pages_end = self.pages + 1

        if pages_end == 1:
            return

        left_end = min(1 + left_edge, pages_end)
        yield from range(1, left_end)

        if left_end == pages_end:
            return

        mid_start = max(left_end, self.page - left_current)
        mid_end = min(self.page + right_current + 1, pages_end)

        if mid_start - left_end > 0:
            yield None

        yield from range(mid_start, mid_end)

        if mid_end == pages_end:
            return

        right_start = max(mid_end, pages_end - right_edge)

        if right_start - mid_end > 0:
            yield None

        yield from range(right_start, pages_end)

    def __iter__(self) -> t.Iterator[t.Any]:
        yield from self.items
